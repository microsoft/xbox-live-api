#pragma once

namespace xbox { namespace services { namespace player_state {

#if TV_API | XBOX_UWP
    typedef  Windows::Xbox::System::User^ xbox_live_user_t;
#else
    typedef std::shared_ptr<xbox::services::system::xbox_live_user> xbox_live_user_t;
#endif

union player_state_value_union
{
    bool boolType;
    double numberType;
    int64_t integerType;
};

/// <summary> 
/// Represents a player state
/// Player state is a set
/// </summary>
class player_state_value
{
public:
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

    /// Internal
    player_state_value();

    /// Internal
    player_state_value(
        _In_ string_t value
        );

    /// Internal
    player_state_value(
        _In_ double value
        );

    /// Internal
    player_state_value(
        _In_ int64_t value
        );

    /// Internal
    player_state_value(
        _In_ bool value
        );

private:
    string_t m_stringData;
    player_state_value_union m_nonStringData;
};

/// <summary> 
/// The player state writer receives information about the player state
/// This information is used for various services like stats_manager, contextual search information, and presence
/// </summary>
class player_state_writer
{
public:
    /// <summary> 
    /// Instantiates and returns an instance of the player state writer
    /// </summary>
    static std::shared_ptr<player_state_writer> get_singleton_instance();

    /// <summary> 
    /// Sets player state. Immediately applies.
    /// </summary>
    xbox_live_result<void> set_player_state(_In_ xbox_live_user_t user, _In_ const std::unordered_map<string_t, player_state_value>& playerStateList);

    /// <summary> 
    /// Gets the player state list
    /// </summary>
    xbox_live_result<void> get_player_state(_In_ xbox_live_user_t user, _Inout_ std::unordered_map<string_t, player_state_value>& playerStateList);

private:
    std::unordered_map<string_t, std::unordered_map<string_t, player_state_value>> m_currentState;
};
    
}}}
