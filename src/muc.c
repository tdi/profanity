/*
 * muc.c
 *
 * Copyright (C) 2012, 2013 James Booth <boothj5@gmail.com>
 *
 * This file is part of Profanity.
 *
 * Profanity is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Profanity is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Profanity.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "contact.h"
#include "jid.h"
#include "prof_autocomplete.h"

typedef struct _muc_room_t {
    char *room; // e.g. test@conference.server
    char *nick; // e.g. Some User
    char *subject;
    gboolean pending_nick_change;
    GHashTable *roster;
    PAutocomplete nick_ac;
    GHashTable *nick_changes;
    gboolean roster_received;
} ChatRoom;

GHashTable *rooms = NULL;

static void _free_room(ChatRoom *room);

/*
 * Join the chat room with the specified nickname
 */
void
muc_join_room(const char * const room, const char * const nick)
{
    if (rooms == NULL) {
        rooms = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
            (GDestroyNotify)_free_room);
    }

    ChatRoom *new_room = malloc(sizeof(ChatRoom));
    new_room->room = strdup(room);
    new_room->nick = strdup(nick);
    new_room->subject = NULL;
    new_room->roster = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
        (GDestroyNotify)p_contact_free);
    new_room->nick_ac = p_autocomplete_new();
    new_room->nick_changes = g_hash_table_new_full(g_str_hash, g_str_equal,
        g_free, g_free);
    new_room->roster_received = FALSE;
    new_room->pending_nick_change = FALSE;

    g_hash_table_insert(rooms, strdup(room), new_room);
}

/*
 * Leave the room
 */
void
muc_leave_room(const char * const room)
{
    g_hash_table_remove(rooms, room);
}

/*
 * Returns TRUE if the user is currently in the room
 */
gboolean
muc_room_is_active(Jid *jid)
{
    if (rooms != NULL) {
        ChatRoom *chat_room = g_hash_table_lookup(rooms, jid->barejid);

        if (chat_room != NULL) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        return FALSE;
    }
}

/*
 * Flag that the user has sent a nick change to the service
 * and is awaiting the response
 */
void
muc_set_room_pending_nick_change(const char * const room)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        chat_room->pending_nick_change = TRUE;
    }
}

/*
 * Returns TRUE if the room is awaiting the result of a
 * nick change
 */
gboolean
muc_is_room_pending_nick_change(const char * const room)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        return chat_room->pending_nick_change;
    } else {
        return FALSE;
    }
}

/*
 * Change the current nuck name for the room, call once
 * the service has responded
 */
void
muc_complete_room_nick_change(const char * const room, const char * const nick)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        free(chat_room->nick);
        chat_room->nick = strdup(nick);
        chat_room->pending_nick_change = FALSE;
    }
}

/*
 * Return a list of room names
 * The contents of the list are owned by the chat room and should not be
 * modified or freed.
 */
GList *
muc_get_active_room_list(void)
{
    if (rooms != NULL) {
        return g_hash_table_get_keys(rooms);
    } else {
        return NULL;
    }
}

/*
 * Return current users nickname for the specified room
 * The nickname is owned by the chat room and should not be modified or freed
 */
char *
muc_get_room_nick(const char * const room)
{
    if (rooms != NULL) {
        ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

        if (chat_room != NULL) {
            return chat_room->nick;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

/*
 * Returns TRUE if the specified nick exists in the room's roster
 */
gboolean
muc_nick_in_roster(const char * const room, const char * const nick)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        PContact contact = g_hash_table_lookup(chat_room->roster, nick);
        if (contact != NULL) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    return FALSE;
}

/*
 * Add a new chat room member to the room's roster
 */
gboolean
muc_add_to_roster(const char * const room, const char * const nick,
    const char * const show, const char * const status)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);
    gboolean updated = FALSE;

    if (chat_room != NULL) {
        PContact old = g_hash_table_lookup(chat_room->roster, nick);

        if (old == NULL) {
            updated = TRUE;
            p_autocomplete_add(chat_room->nick_ac, strdup(nick));
        } else if ((g_strcmp0(p_contact_presence(old), show) != 0) ||
                    (g_strcmp0(p_contact_status(old), status) != 0)) {
            updated = TRUE;
        }

        PContact contact = p_contact_new(nick, NULL, show, status, NULL, FALSE);
        g_hash_table_replace(chat_room->roster, strdup(nick), contact);
    }

    return updated;
}

/*
 * Remove a room member from the room's roster
 */
void
muc_remove_from_roster(const char * const room, const char * const nick)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        g_hash_table_remove(chat_room->roster, nick);
        p_autocomplete_remove(chat_room->nick_ac, nick);
    }
}

PContact
muc_get_participant(const char * const room, const char * const nick)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        PContact participant = g_hash_table_lookup(chat_room->roster, nick);
        return participant;
    }

    return NULL;
}

/*
 * Return a list of PContacts representing the room members in the room's roster
 * The list is owned by the room and must not be mofified or freed
 */
GList *
muc_get_roster(const char * const room)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        return g_hash_table_get_values(chat_room->roster);
    } else {
        return NULL;
    }
}

/*
 * Return a PAutocomplete representing the room member's in the roster
 */
PAutocomplete
muc_get_roster_ac(const char * const room)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        return chat_room->nick_ac;
    } else {
        return NULL;
    }
}

/*
 * Set to TRUE when the rooms roster has been fully recieved
 */
void
muc_set_roster_received(const char * const room)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        chat_room->roster_received = TRUE;
    }
}

/*
 * Returns TRUE id the rooms roster has been fully recieved
 */
gboolean
muc_get_roster_received(const char * const room)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        return chat_room->roster_received;
    } else {
        return FALSE;
    }
}

/*
 * Remove the old_nick from the roster, and flag that a pending nickname change
 * is in progress
 */
void
muc_set_roster_pending_nick_change(const char * const room,
    const char * const new_nick, const char * const old_nick)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        g_hash_table_insert(chat_room->nick_changes, strdup(new_nick), strdup(old_nick));
        muc_remove_from_roster(room, old_nick);
    }
}

/*
 * Complete the pending nick name change for a contact in the room's roster
 * The new nick name will be added to the roster
 * The old nick name will be returned in a new string which must be freed by
 * the caller
 */
char *
muc_complete_roster_nick_change(const char * const room,
    const char * const nick)
{
    ChatRoom *chat_room = g_hash_table_lookup(rooms, room);

    if (chat_room != NULL) {
        char *old_nick = g_hash_table_lookup(chat_room->nick_changes, nick);
        char *old_nick_cpy;

        if (old_nick != NULL) {
            old_nick_cpy = strdup(old_nick);
            g_hash_table_remove(chat_room->nick_changes, nick);

            return old_nick_cpy;
        }
    }

    return NULL;
}

static void
_free_room(ChatRoom *room)
{
    if (room != NULL) {
        if (room->room != NULL) {
            g_free(room->room);
            room->room = NULL;
        }
        if (room->nick != NULL) {
            g_free(room->nick);
            room->nick = NULL;
        }
        if (room->subject != NULL) {
            g_free(room->subject);
            room->subject = NULL;
        }
        if (room->roster != NULL) {
            g_hash_table_remove_all(room->roster);
            room->roster = NULL;
        }
        if (room->nick_ac != NULL) {
            p_autocomplete_free(room->nick_ac);
        }
        if (room->nick_changes != NULL) {
            g_hash_table_remove_all(room->nick_changes);
            room->nick_changes = NULL;
        }
        g_free(room);
    }
    room = NULL;
}