// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** The default access group Xbox Live Services uses for shared data. */
extern NSString *const _Nonnull XBLKeychainStorageSharedAccessGroup;

/** Interface for accessing the keychain on iOS. */
@interface XBLKeychainStorage : NSObject

/** Gets the access group this storage object is pointing to. */
@property (readonly, nonatomic, nonnull) NSString *accessGroup;

/** Gets and sets the label to use to filter read items and to use to mark written items with. Use nil to indicate read
 all items and to write no label. */
@property (copy, nonatomic, nullable) NSString *itemLabel;

/**
 *  Do not use. Please use one of the other initializers.
 */
- (instancetype _Nonnull)init NS_UNAVAILABLE;

/**
 Initializes the keychain storage object to put items into the default access group of the current running app.
 */
- (instancetype _Nonnull)initWithDefaultAccessGroup;

/**
 Initializes the keychain storage object with a given access group. The identifier prefix is prepended
 automatically.
 
 @param accessGroup The access group to use for this keychain storage object or nil to not specify one and allow the
 keychain service to use its default behavior.
 */
- (instancetype _Nonnull)initWithAccessGroup:(NSString * _Nullable)accessGroup;

/**
 Initializes the keychain storage with a specific label.
 
 @param label Value to be set for kSecAttrLabel to make it easy to query for items in keychain.
 */
- (instancetype _Nonnull)initWithDefaultAccessGroupAndLabel:(NSString * _Nullable)label;

/**
 Initializes the keychain storage with a given access group and label.
 
 @param accessGroup The access group to use for this keychain storage object or nil to not specify one and allow the
 keychain service to use its default behavior.
 @param label Value to be set for kSecAttrLabel to make it easy to query for items in keychain.
 */
- (instancetype _Nonnull)initWithAccessGroup:(NSString * _Nullable)accessGroup
                                       label:(NSString * _Nullable)label;

/**
 Adds or updates an item to the keychain under the specified key. If an item does not already exist at the key
 one will be added. If an item does already exist at the key, it will be updated. Returns YES if the operation succeeds
 and NO with a corresponding error if it does not.
 
 @param key The key to store the item under.
 @param item Archivable item to save.
 @param error optional: The error pointer to assign upon error or nil if not wanted.
 @return YES if the operation succeeds and NO with an accompanying error if it does not.
 */
- (BOOL)addOrUpdateItemForKey:(NSString * _Nonnull)key
                         item:(id _Nonnull)item
                        error:(out NSError * _Nullable __autoreleasing * _Nullable)error;

/**
 Retrieves the attributes for the item with the given key. If the item is missing, the out parameter is set to
 nil and YES is returned.
 
 @param key The key to query the attributes for.
 @param attributes The out param to which a new dictionary will be assigned.
 @param error optional: The error pointer to assign upon error or nil if not wanted.
 @return YES if the operation succeeds and NO with an accompanying error if it does not.
 */
- (BOOL)getAttributesForKey:(NSString * _Nonnull)key
                 attributes:(out NSDictionary * _Nullable __autoreleasing * _Nonnull)attributes
                      error:(out NSError * _Nullable __autoreleasing * _Nullable)error;

/**
 Reads and unarchives the data at the specified key. Returns YES if the operation succeeds and NO with a
 corresponding error if it does not. If the item is missing, the out parameter is set to nil and YES is returned.
 
 @param key The key to read the item under.
 @param item The id to assign upon unarchiving.
 @param error optional: The error pointer to assign upon error or nil if not wanted.
 @return YES if the operation succeeds and NO with an accompanying error if it does not.
 */
- (BOOL)readItemForKey:(NSString * _Nonnull)key
                  item:(out id _Nullable __autoreleasing * _Nonnull)item
                 error:(out NSError * _Nullable __autoreleasing * _Nullable)error;

/**
 Deletes the item at the specified index if it exists.
 
 @param key The key of the item to delete.
 @param error optional: The error pointer to assign upon error or nil if not wanted.
 @return YES if the operation succeeds and NO with an accompanying error if it does not.
 */
- (BOOL)deleteItemForKey:(NSString * _Nonnull)key
                   error:(out NSError * _Nullable __autoreleasing * _Nullable)error;

/**
 Reads and unarchives the data for the current access group and label combination. Returns YES if the operation
 succeeds and NO with a corresponding error if it does not. If there are no items in the access group, the out parameter
 NSArray is set to empty array and YES is returned.
 
 @param items An array of items in the access group.
 @param error optional: The error pointer to assign upon error or nil if not wanted.
 @return YES if the operation succeeds and NO with an accompanying error if it does not.
 */
- (BOOL)readAllItemsForAccessGroup:(out NSArray<id> * _Nullable __autoreleasing * _Nonnull)items
                             error:(out NSError * _Nullable __autoreleasing * _Nullable)error;

/**
 Reads all the keys in the current access group and label combination. Returns YES on success and NO with an
 error object if the operation fails. If no keys are found, an empty array is returned.
 
 @param keys An array of existing keys.
 @param error optional: The error pointer to assign upon error or nil if not wanted.
 @return YES if the operation succeeds and NO with an accompanying error if it does not.
 */
- (BOOL)getAllKeys:(out NSArray<NSString *> * _Nullable __autoreleasing * _Nonnull)keys
             error:(out NSError * _Nullable __autoreleasing * _Nullable)error;

@end
