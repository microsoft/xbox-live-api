// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#import "XBLKeychainStorage.h"
#import "XBLServiceManager.h"

NSString *const _Nonnull XBLKeychainStorageSharedAccessGroup = @"com.microsoft.xboxliveservices";

/** The service name to use for items stored in the keychain by Xbox Live Services. */
static NSString *const XBLKeychainStorageServiceName = @"com.microsoft.xboxliveservices";

/** The application identifier prefix for the current running app. Note: this should only be accessed via
 getApplicationPrefix. */
static NSString *XBLKeychainStorageApplicationIdentifierPrefix = nil;

@implementation XBLKeychainStorage

- (instancetype _Nonnull)initWithDefaultAccessGroup {
    return [self initWithAccessGroup:nil label:nil];
}

- (instancetype _Nonnull)initWithAccessGroup:(NSString * _Nullable)accessGroup; {
    return [self initWithAccessGroup:accessGroup label:nil];
}

- (instancetype _Nonnull)initWithDefaultAccessGroupAndLabel:(NSString * _Nullable)label {
   return [self initWithAccessGroup:nil label:label];
}

- (instancetype _Nonnull)initWithAccessGroup:(NSString * _Nullable)accessGroup
                                       label:(NSString * _Nullable)label {
    self = [super init];
    if (self != nil) {
        if (accessGroup != nil) {
            _accessGroup = [NSString stringWithFormat:@"%@.%@", [XBLKeychainStorage getApplicationPrefix], accessGroup];
        }
        
        _itemLabel = [label copy];
    }
    
    return self;
}

/**
 Retrieves the application identifier prefix for use with accessing keychain groups.
 */
+ (NSString * _Nonnull)getApplicationPrefix {
    if (XBLKeychainStorageApplicationIdentifierPrefix == nil) {
        // We have to call this method to get the prefix initialized as well.
        [XBLKeychainStorage getApplicationAccessGroup];
        
        LOG_DEBUG(([[NSString stringWithFormat:@"Application bundle prefix detected: %@",
                     XBLKeychainStorageApplicationIdentifierPrefix] UTF8String]));
    }
    
    return XBLKeychainStorageApplicationIdentifierPrefix;
}

/**
 Retrieves the application's default access group. In the case of a simulator runtime, this might return an empty
 string.
 
 @return The application's default access group specified in the application's entitlements file.
 */
+ (NSString * _Nonnull)getApplicationAccessGroup {
    static NSString *keychainStorageApplicationAccessGroup = nil;
    
    // Perform the detection inside of a dispatch_once block. We don't expect any failures but there's no reason to kill
    // the app if one step fails.
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        // Add a unique item to the keychain. The item will be added with the app's default access group.
        XBLKeychainStorage *keychain = [[XBLKeychainStorage alloc] initWithAccessGroup:nil];
        
        NSString *uniqueKey = [NSString stringWithFormat:@"AccessGroupDetection-%@",
                               [[NSUUID UUID] UUIDString]];
        
        NSError *error = nil;
        if (![keychain addOrUpdateItemForKey:uniqueKey item:uniqueKey error:&error]) {
            NSAssert(NO,
                     @"Error when adding new unique item %@ to the keychain: %@",
                     uniqueKey,
                     error);
            
            return;
        }
        
        NSString *applicationAccessGroup = nil;
        // Retrieve the unique item's access group.
        if (![keychain retrieveItemAccessGroupForKey:uniqueKey
                                         accessGroup:&applicationAccessGroup
                                               error:&error]) {
            NSAssert(NO,
                     @"Error when reading the default access group: %@",
                     error);
            
            return;
        }
        
        // For signed apps this will come back as the first entry in the entitlements file or in an
        // unsigned build it should come back simply as @"test".
        NSAssert([applicationAccessGroup length] > 0,
                 @"Default access group came back as empty: %@",
                 applicationAccessGroup);
        
        NSArray *components = [applicationAccessGroup componentsSeparatedByString:@"."];
        XBLKeychainStorageApplicationIdentifierPrefix = [[components objectEnumerator] nextObject];
        
        keychainStorageApplicationAccessGroup = [applicationAccessGroup substringFromIndex:
                                                 XBLKeychainStorageApplicationIdentifierPrefix.length];
        if ([keychainStorageApplicationAccessGroup hasPrefix:@"."]) {
            // Remove the first '.' from the default group name
            keychainStorageApplicationAccessGroup = [keychainStorageApplicationAccessGroup
                                                     substringFromIndex:1];
        }
        
        // Delete the unique item.
        if (![keychain deleteItemForKey:uniqueKey error:&error]) {
            NSAssert(NO,
                     @"Error when deleting unique item %@ from the keychain: %@",
                     uniqueKey,
                     error);
            
            return;
        }
        
        LOG_DEBUG(([[NSString stringWithFormat:@"Default app access group detected: %@",
                     keychainStorageApplicationAccessGroup] UTF8String]));
    });
    
    return keychainStorageApplicationAccessGroup;
}

/**
 Reads the access group for the specified key. YES with a nil access group is returned if the item cannot be
 found.
 
 @param key The key of the item to examine.
 @param accessGroup The out parameter the access group is returned through.
 @param error optional: The error object describing the failure that occurred if one occurred.
 @return YES if the call succeeds. Otherwise NO is returned along with a populated error object.
 */
- (BOOL)retrieveItemAccessGroupForKey:(NSString * _Nonnull)key
                          accessGroup:(out NSString * _Nullable __autoreleasing * _Nonnull)accessGroup
                                error:(out NSError * _Nullable __autoreleasing * _Nullable)error {
    NSAssert(accessGroup != nil, @"Out parameter must be non-nil.");
    
    // Build the query to search for the requested item.
    NSMutableDictionary *query = [self dictionaryForKeychainQuery:key];
    query[(__bridge id)kSecReturnAttributes] = (__bridge id)kCFBooleanTrue;
    
    // Search for it.
    CFDictionaryRef cfAttributes = NULL;
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&cfAttributes);
    if (status == errSecSuccess) {
        NSDictionary *attributes = (__bridge_transfer NSDictionary *)cfAttributes;
        *accessGroup = attributes[(__bridge id)kSecAttrAccessGroup];
        
        NSAssert([*accessGroup length] > 0, @"The access group should be non-empty: %@", *accessGroup);
        if ([*accessGroup length] == 0) {
            // For compiled versions of the library we ship, make sure that if this is running in an environment where
            // access groups are empty, we should still behave nicely
            *accessGroup = nil;
        }
    } else if (status == errSecItemNotFound) {
        // Item not found. Set return to nil and still call this a success.
        *accessGroup = nil;
    } else {
        NSAssert(NO, @"Error loading keychain item: %d", (int)status);
        
        if (error != nil) {
            *error = [NSError errorWithDomain:kXboxLiveServicesDomain
                                         code:status
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat:@"Error occurred while loading keychain item \"%@\" for retrieving access group.",
                                                                            key]}];
        }
        
        return NO;
    }
    
    return YES;
}

- (BOOL)addOrUpdateItemForKey:(NSString * _Nonnull)key
                         item:(id _Nonnull)item
                        error:(out NSError * _Nullable __autoreleasing * _Nullable)error {
    NSAssert(item != nil, @"Trying to write a nil item to keychain. This probably isn't intended. key: %@", key);
    
    // First find out if the item already exists
    NSMutableDictionary *query = [self dictionaryForKeychainQuery:key];
    
    // For the initial query we don't want to use a label because the caller is asking to update the item. This should
    // be performed even if an existing item with a different label exists. If the item does exist, the label will be
    // added as part of the update dictionary. If the item does not exist, the query dictionary will be replaced
    // entirely.
    [query removeObjectForKey:(__bridge id)kSecAttrLabel];
    
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, NULL);
    if (status == errSecSuccess) {
        LOG_DEBUG(([[NSString stringWithFormat:@"Updating existing item in keychain: %@", key] UTF8String]));
        
        // The item exists, perform an update
        NSMutableDictionary *update = [self dictionaryForKeychainAddOrUpdate:key];
        
        // Set the data on the update
        update[(__bridge id)kSecValueData] = [NSKeyedArchiver archivedDataWithRootObject:item];
        
        // The class key must be removed from the update because... dragons?
        [update removeObjectForKey:(__bridge id)kSecClass];
        
        status = SecItemUpdate((__bridge CFDictionaryRef)query, (__bridge CFDictionaryRef)update);
        
        if (status == errSecItemNotFound) {
            // If we get errSecItemNotFound, assume the item was deleted by another thread and allow the deleter to win
            // the last-write-wins game.
            LOG_WARN("Item we were trying to update no longer exists.");
        } else if (status != errSecSuccess) {
            if (error != nil) {
                *error = [NSError errorWithDomain:kXboxLiveServicesDomain
                                             code:status
                                         userInfo:@{NSLocalizedDescriptionKey: @"Error occurred while updating keychain item."}];
            }
            
            return NO;
        }
    } else if (status == errSecItemNotFound) {
        LOG_DEBUG(([[NSString stringWithFormat:@"Adding new item in keychain: %@", key] UTF8String]));
        
        // The item does not exist, perform an add
        query = [self dictionaryForKeychainAddOrUpdate:key];
        
        query[(__bridge id)kSecValueData] = [NSKeyedArchiver archivedDataWithRootObject:item];
        status = SecItemAdd((__bridge CFDictionaryRef)query, NULL);
        
        if (status == errSecDuplicateItem) {
            // If we get errSecDuplicateItem assume the item was added by another thread and allow them to win the
            // last-write-wins game.
            LOG_WARN("Item we were trying to add is now a duplicate.");
        } else if (status != errSecSuccess) {
            if (error != nil) {
                *error = [NSError errorWithDomain:kXboxLiveServicesDomain
                                             code:status
                                         userInfo:@{NSLocalizedDescriptionKey: @"Error occurred while trying to add keychain item."}];
            }
            
            return NO;
        }
    } else {
        // Unexpected error
        
        if (error != nil) {
            *error = [NSError errorWithDomain:kXboxLiveServicesDomain
                                         code:status
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat:@"Error occurred while checking the status of a keychain item \"%@\".",
                                                                            key]}];
        }
        
        return NO;
    }
    
    return YES;
}

- (BOOL)getAttributesForKey:(NSString * _Nonnull)key
                 attributes:(out NSDictionary * _Nullable __autoreleasing * _Nonnull)attributes
                      error:(out NSError * _Nullable __autoreleasing * _Nullable)error {
    NSAssert(key.length > 0, @"key must be non-empty.");
    NSAssert(attributes != nil, @"Out param must be non-nil.");
    
    BOOL success = YES;
    CFArrayRef itemAttributes = NULL;
    
    NSMutableDictionary *query = [self dictionaryForKeychainQuery:key];
    query[(__bridge id)kSecReturnAttributes] = (__bridge id)kCFBooleanTrue;
    query[(__bridge id)kSecMatchLimit] = (__bridge id)kSecMatchLimitOne;
    
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&itemAttributes);
    if (status == errSecSuccess) {
        NSAssert(itemAttributes != NULL, @"Attributes expected to be non-nil on success.");
        
        *attributes = (__bridge_transfer NSDictionary*)itemAttributes;
    } else if (status == errSecItemNotFound) {
        *attributes = nil;
    } else {
        if (error != nil) {
            *error = [NSError errorWithDomain:kXboxLiveServicesDomain
                                         code:status
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat:@"Error occurred while reading attributes for item \"%@\" from keychain.",
                                                                            key]}];
        }
        
        success = NO;
    }
    
    return success;
}

- (BOOL)readItemForKey:(NSString * _Nonnull)key
                  item:(out id _Nullable __autoreleasing * _Nonnull)item
                 error:(out NSError * _Nullable __autoreleasing * _Nullable)error {
    NSAssert(item != nil, @"Out param must be non-nil.");
    
    BOOL success = YES;
    CFDataRef itemData = NULL;
    
    NSMutableDictionary *query = [self dictionaryForKeychainQuery:key];
    query[(__bridge id)kSecReturnData] = (__bridge id)kCFBooleanTrue;
    query[(__bridge id)kSecMatchLimit] = (__bridge id)kSecMatchLimitOne;
    
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&itemData);
    if (status == errSecSuccess) {
        NSAssert(itemData != NULL, @"Data expected to be non-nil on success.");
        
        *item = [NSKeyedUnarchiver unarchiveObjectWithData:(__bridge NSData *)itemData];
    } else if (status == errSecItemNotFound) {
        *item = nil;
    } else {
        if (error != nil) {
            *error = [NSError errorWithDomain:kXboxLiveServicesDomain
                                         code:status
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat:@"Error occurred while reading item \"%@\" from keychain.",
                                                                            key]}];
        }
        
        // Don't return right away so that we make sure to clean up itemData.
        // In theory there should be nothing to clean up if an error occurred but this is safer.
        success = NO;
    }
    
    if (itemData != NULL) {
        CFRelease(itemData);
    }
    
    return success;
}

- (BOOL)deleteItemForKey:(NSString * _Nonnull)key
                   error:(out NSError * _Nullable __autoreleasing * _Nullable)error {
    NSMutableDictionary *query = [self dictionaryForKeychainQuery:key];
    OSStatus status = SecItemDelete((__bridge CFDictionaryRef)query);
    if (status != errSecSuccess && status != errSecItemNotFound) {
        if (error != nil) {
            *error = [NSError errorWithDomain:kXboxLiveServicesDomain
                                         code:status
                                     userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat:@"Error occurred while deleting item \"%@\" from keychain.",
                                                                            key]}];
        }
        
        return NO;
    }
    
    return YES;
}

- (BOOL)readAllItemsForAccessGroup:(out NSArray<id> * _Nullable __autoreleasing * _Nonnull)items
                             error:(out NSError * _Nullable __autoreleasing * _Nullable)error {
    NSAssert(items != nil, @"Out param must be non-nil.");
             
    BOOL success = YES;
    CFArrayRef itemData = NULL;
    
    NSMutableDictionary *query = [self dictionaryForKeychainQuery:nil];
    query[(__bridge id)kSecReturnData] = (__bridge id)kCFBooleanTrue;
    query[(__bridge id)kSecMatchLimit] = (__bridge id)kSecMatchLimitAll;
    
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&itemData);
    
    if (status == errSecSuccess) {
        NSAssert(itemData != NULL, @"Data expected to be non-nil on success.");
        
        NSArray *itemArray = (__bridge_transfer NSArray *)itemData;
        
        NSMutableArray<id> *valueData = [[NSMutableArray alloc] initWithCapacity:itemArray.count];
        
        for (NSData *item in itemArray) {
            id decodedItem = [NSKeyedUnarchiver unarchiveObjectWithData:item];
            
            // For unknown reasons, it's possible for the item to come out as nil
            if (decodedItem != nil) {
                [valueData addObject:decodedItem];
            } else {
                LOG_ERROR(([[NSString stringWithFormat:@"Decoded value from keychain resulted in nil: %@",
                             (item == nil) ? @"nil" : @"non-nil"] UTF8String]));
                NSAssert(NO, @"A keychain item is either nil or could not be unarchived.");
            }
        }
        
        *items = [valueData copy];
    } else if (status == errSecItemNotFound) {
        *items = [NSArray array];
    } else {
        if (error != nil) {
            *error = [NSError errorWithDomain:kXboxLiveServicesDomain
                                         code:status
                                     userInfo:@{NSLocalizedDescriptionKey: @"Error occurred while reading items from keychain."}];
        }
        
        success = NO;
    }
    
    return success;
}

- (BOOL)getAllKeys:(out NSArray<NSString *> * _Nullable __autoreleasing * _Nonnull)keys
             error:(out NSError * __autoreleasing * _Nullable)error {
    NSAssert(keys != nil, @"Out param must be non-nil.");
    
    BOOL success = YES;
    CFArrayRef itemAttributes = NULL;
    
    NSMutableDictionary *query = [self dictionaryForKeychainQuery:nil];
    query[(__bridge id)kSecReturnAttributes] = (__bridge id)kCFBooleanTrue;
    query[(__bridge id)kSecMatchLimit] = (__bridge id)kSecMatchLimitAll;
    
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, (CFTypeRef *)&itemAttributes);
    
    if (status == errSecSuccess) {
        NSAssert(itemAttributes != NULL, @"Attributes expected to be non-nil on success.");
        
        NSArray *attributesArray = (__bridge_transfer NSArray *)itemAttributes;
        
        NSMutableArray<NSString *> *mutableKeyList = [[NSMutableArray alloc] initWithCapacity:attributesArray.count];
        
        for (NSDictionary *attributes in attributesArray) {
            NSString *key = attributes[(__bridge id)kSecAttrAccount];
            if (key.length > 0) {
                [mutableKeyList addObject:[key copy]];
            }
        }
        
        *keys = [mutableKeyList copy];
    } else if (status == errSecItemNotFound) {
        *keys = [NSArray array];
    } else {
        if (error != nil) {
            *error = [NSError errorWithDomain:kXboxLiveServicesDomain
                                         code:status
                                     userInfo:@{NSLocalizedDescriptionKey: @"Error occurred while reading attributes from keychain."}];
        }
        
        success = NO;
    }
    
    return success;
}

/**
 Creates a keychain query for writing or updating the given key.
 
 @param key The key to taget the query to.
 @return A keychain query dictionary.
 */
- (NSMutableDictionary * _Nonnull)dictionaryForKeychainAddOrUpdate:(NSString * _Nonnull)key {
    NSMutableDictionary *query = [self dictionaryForKeychainQuery:key];

    query[(__bridge id)kSecAttrAccessible] = (__bridge id)kSecAttrAccessibleAfterFirstUnlock;
    query[(__bridge id)kSecAttrIsInvisible] = (__bridge id)kCFBooleanTrue;
    
    return query;
}

/**
 Creates a keychain query for reading or deleting the given key.
 
 @param key The key to target the query to.
 @return A keychain query dictionary.
 */
- (NSMutableDictionary * _Nonnull)dictionaryForKeychainQuery:(NSString * _Nonnull)key {
    NSMutableDictionary *query = [NSMutableDictionary dictionary];
    
    query[(__bridge id)kSecClass] = (__bridge id)kSecClassGenericPassword;
    query[(__bridge id)kSecAttrService] = XBLKeychainStorageServiceName;
    query[(__bridge id)kSecAttrAccount] = key;
    
    if (_itemLabel != nil) {
        query[(__bridge id)kSecAttrLabel] = _itemLabel;
    }
    
    if (_accessGroup != nil) {
        query[(__bridge id)kSecAttrAccessGroup] = _accessGroup;
    }
    
    return query;
}

@end
