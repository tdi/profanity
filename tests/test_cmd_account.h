void cmd_account_shows_usage_when_not_connected_and_no_args(void **state);
void cmd_account_shows_account_when_connected_and_no_args(void **state);
void cmd_account_list_shows_accounts(void **state);
void cmd_account_show_shows_usage_when_no_arg(void **state);
void cmd_account_show_shows_message_when_account_does_not_exist(void **state);
void cmd_account_show_shows_account_when_exists(void **state);
void cmd_account_add_shows_usage_when_no_arg(void **state);
void cmd_account_add_adds_account(void **state);
void cmd_account_add_shows_message(void **state);
void cmd_account_enable_shows_usage_when_no_arg(void **state);
void cmd_account_enable_enables_account(void **state);
void cmd_account_enable_shows_message_when_enabled(void **state);
void cmd_account_enable_shows_message_when_account_doesnt_exist(void **state);
void cmd_account_disable_shows_usage_when_no_arg(void **state);
void cmd_account_disable_disables_account(void **state);
void cmd_account_disable_shows_message_when_disabled(void **state);
void cmd_account_disable_shows_message_when_account_doesnt_exist(void **state);
void cmd_account_rename_shows_usage_when_no_args(void **state);
void cmd_account_rename_shows_usage_when_one_arg(void **state);
void cmd_account_rename_renames_account(void **state);
void cmd_account_rename_shows_message_when_renamed(void **state);
void cmd_account_rename_shows_message_when_not_renamed(void **state);
void cmd_account_set_shows_usage_when_no_args(void **state);
void cmd_account_set_shows_usage_when_one_arg(void **state);
void cmd_account_set_shows_usage_when_two_args(void **state);
void cmd_account_set_checks_account_exists(void **state);
void cmd_account_set_shows_message_when_account_doesnt_exist(void **state);
void cmd_account_set_jid_shows_message_for_malformed_jid(void **state);
void cmd_account_set_jid_sets_barejid(void **state);
void cmd_account_set_jid_sets_resource(void **state);
void cmd_account_set_server_sets_server(void **state);
void cmd_account_set_server_shows_message(void **state);
void cmd_account_set_resource_sets_resource(void **state);
void cmd_account_set_resource_shows_message(void **state);
void cmd_account_set_password_sets_password(void **state);
void cmd_account_set_password_shows_message(void **state);
void cmd_account_set_muc_sets_muc(void **state);
void cmd_account_set_muc_shows_message(void **state);
void cmd_account_set_nick_sets_nick(void **state);
void cmd_account_set_nick_shows_message(void **state);
void cmd_account_set_status_shows_message_when_invalid_status(void **state);
void cmd_account_set_status_sets_status_when_valid(void **state);
void cmd_account_set_status_sets_status_when_last(void **state);
void cmd_account_set_status_shows_message_when_set_valid(void **state);
void cmd_account_set_status_shows_message_when_set_last(void **state);
void cmd_account_set_invalid_presence_string_priority_shows_message(void **state);
void cmd_account_set_last_priority_shows_message(void **state);
void cmd_account_set_online_priority_sets_preference(void **state);
void cmd_account_set_chat_priority_sets_preference(void **state);
void cmd_account_set_away_priority_sets_preference(void **state);
void cmd_account_set_xa_priority_sets_preference(void **state);
void cmd_account_set_dnd_priority_sets_preference(void **state);
void cmd_account_set_online_priority_shows_message(void **state);
void cmd_account_set_priority_too_low_shows_message(void **state);
void cmd_account_set_priority_too_high_shows_message(void **state);
void cmd_account_set_priority_when_not_number_shows_message(void **state);
void cmd_account_set_priority_when_empty_shows_message(void **state);
void cmd_account_set_priority_updates_presence_when_account_connected_with_presence(void **state);
void cmd_account_clear_shows_usage_when_no_args(void **state);
void cmd_account_clear_shows_usage_when_one_arg(void **state);
void cmd_account_clear_checks_account_exists(void **state);
void cmd_account_clear_shows_message_when_account_doesnt_exist(void **state);
void cmd_account_clear_shows_message_when_invalid_property(void **state);