#pragma once

namespace xbox { namespace services { namespace player_state_writer {

/// <summary> 
/// Event type
/// </summary>
enum class player_state_event_type
{
    local_user_added,
    local_user_removed
};

/// <summary> 
/// Represents a player state
/// Player state is a set
/// </summary>
class player_state
{
public:
    player_state(
        _In_ const string_t& name,
        _In_ const string_t& value
        );

    player_state(
        _In_ const string_t& name,
        _In_ double value
        );

    player_state(
        _In_ const string_t& name,
        _In_ int64_t value
        );

    player_state(
        _In_ const string_t& name,
        _In_ bool value
        );

    /// <summary> 
    /// Name of player state set
    /// </summary>
    const string_t& name() const;

    /// <summary> 
    /// Gets the value as a string
    /// </summary>
    string_t as_string() const;

    /// <summary> 
    /// Gets the value as a boolean
    /// </summary>
    bool as_bool() const;

    /// <summary> 
    /// Gets the value as a boolean
    /// </summary>
    double as_number() const;

    /// <summary> 
    /// Gets the value as a boolean
    /// </summary>
    int64_t as_integer() const;
};

/// <summary> 
/// Player state event
/// </summary>
class player_state_event
{
public:
    /// <summary> 
    /// Represents error code and error message
    /// </summary>
    /// <return>The returned xbox live result</return>
    const xbox_live_result<void>& error_info() const;

    /// <summary> 
    /// The type of event the statistic is
    /// </summary>
    /// <return>The event type</return>
    player_state_event_type event_type() const;

    /// <summary> 
    /// Local user the event is for
    /// </summary>
    /// <return>The returned user</return>
    const xbox_live_user_t& local_user() const;

    /// Internal function
    player_state_event(
        player_state_event_type eventType,
        xbox_live_user_t user,
        xbox_live_result<void> errorInfo
        );
};

/// <summary> 
/// The player state writer expects information for various player state information
/// This information is used for various services like stats_manager, contextual search information, and presence
/// </summary>
class player_state_writer
{
public:
    /// <summary> 
    /// Instantiates and returns an instance of the player state writer
    /// </summary>
    static player_state_writer get_singleton_instance();

    /// <summary> 
    /// Adds a local user to the player state writer
    /// Returns a local_user_added event from do_work
    /// </summary>
    /// <param name="user">The user to add to the player state writer</param>
    xbox_live_result<void> add_local_user(_In_ const xbox_live_user_t& user);

    /// <summary> 
    /// Removes a local user from the player state writer
    /// Returns a local_user_removed event from do_work
    /// </summary>
    /// <param name="user">The user to be removed from the player state writer</param>
    xbox_live_result<void> remove_local_user(_In_ const xbox_live_user_t& user);

    /// <summary> 
    /// Returns any events that have been processed
    /// </summary>
    /// <return>A list of events that have happened since previous do_work</return>
    std::vector<player_state_event> do_work();

    /// <summary> 
    /// Sets player state. On do_work, player state is set
    /// </summary>
    /// <return>A list of events that have happened since previous do_work</return>
    xbox_live_result<void> set_player_state(_In_ const player_state& playerState);

    /// <summary> 
    /// Removes a player state object from the internal list
    /// </summary>
    xbox_live_result<void> remove_player_state(_In_ const string_t& name);

    /// <summary> 
    /// Removes a player state object from the internal list
    /// </summary>
    xbox_live_result<void> remove_player_state(_In_ const player_state& name);

    /// <summary> 
    /// Gets the player state list
    /// </summary>
    xbox_live_result<void> get_player_states(_Inout_ std::vector<player_state>& playerStateList);
};
    
}}}
