#ifndef DEFAULT_SQL_H
#define DEFAULT_SQL_H

static const char* match_default_mysql = "CREATE TABLE `match` (\
    `match_id` bigint(20) unsigned NOT NULL,\
    `duration` smallint(5) unsigned DEFAULT NULL,\
    `pre_game_duration` smallint(5) unsigned DEFAULT NULL,\
    `radiant_win` tinyint(3) unsigned DEFAULT NULL,\
    `start_time` int(10) unsigned DEFAULT NULL,\
    `match_seq_num` int(10) unsigned DEFAULT NULL,\
    `tower_status_radiant` smallint(5) unsigned DEFAULT NULL,\
    `tower_status_dire` smallint(5) unsigned DEFAULT NULL,\
    `barracks_status_radiant` tinyint(3) unsigned DEFAULT NULL,\
    `barracks_status_dire` tinyint(3) unsigned DEFAULT NULL,\
    `cluster` smallint(5) unsigned DEFAULT NULL,\
    `first_blood_time` smallint(5) unsigned DEFAULT NULL,\
    `lobby_type` tinyint(4) DEFAULT NULL,\
    `human_players` tinyint(3) unsigned DEFAULT NULL,\
    `leagueid` int(10) unsigned DEFAULT NULL,\
    `positive_votes` smallint(5) unsigned DEFAULT NULL,\
    `negative_votes` smallint(5) unsigned DEFAULT NULL,\
    `game_mode` tinyint(3) unsigned DEFAULT NULL,\
    `flags` int(10) unsigned DEFAULT NULL,\
    `engine` tinyint(3) unsigned DEFAULT NULL,\
    `radiant_score` smallint(5) unsigned DEFAULT NULL,\
    `dire_score` smallint(5) unsigned DEFAULT NULL,\
    PRIMARY KEY (`match_id`)\
) ENGINE=InnoDB DEFAULT CHARSET=utf8;";

static const char* match_players_default_mysql = "CREATE TABLE `match_players` (\
    `account_id` int(10) unsigned NOT NULL,\
    `match_id` bigint(20) unsigned NOT NULL,\
    `player_slot` tinyint(3) unsigned DEFAULT NULL,\
    `hero_id` smallint(5) unsigned DEFAULT NULL,\
    `item_0` smallint(5) unsigned DEFAULT NULL,\
    `item_1` smallint(5) unsigned DEFAULT NULL,\
    `item_2` smallint(5) unsigned DEFAULT NULL,\
    `item_3` smallint(5) unsigned DEFAULT NULL,\
    `item_4` smallint(5) unsigned DEFAULT NULL,\
    `item_5` smallint(5) unsigned DEFAULT NULL,\
    `backpack_0` smallint(5) unsigned DEFAULT NULL,\
    `backpack_1` smallint(5) unsigned DEFAULT NULL,\
    `backpack_2` smallint(5) unsigned DEFAULT NULL,\
    `kills` smallint(5) unsigned DEFAULT NULL,\
    `deaths` smallint(5) unsigned DEFAULT NULL,\
    `assists` smallint(5) unsigned DEFAULT NULL,\
    `leaver_status` tinyint(3) unsigned DEFAULT NULL,\
    `last_hits` smallint(5) unsigned DEFAULT NULL,\
    `denies` smallint(5) unsigned DEFAULT NULL,\
    `gold_per_min` smallint(5) unsigned DEFAULT NULL,\
    `xp_per_min` smallint(5) unsigned DEFAULT NULL,\
    `level` tinyint(3) unsigned DEFAULT NULL,\
    `hero_damage` mediumint(8) unsigned DEFAULT NULL,\
    `tower_damage` mediumint(8) unsigned DEFAULT NULL,\
    `hero_healing` mediumint(8) unsigned DEFAULT NULL,\
    `gold` mediumint(8) unsigned DEFAULT NULL,\
    `gold_spent` mediumint(8) unsigned DEFAULT NULL,\
    `scaled_hero_damage` mediumint(8) unsigned DEFAULT NULL,\
    `scaled_tower_damage` mediumint(8) unsigned DEFAULT NULL,\
    `scaled_hero_healing` mediumint(8) unsigned DEFAULT NULL,\
    PRIMARY KEY (`match_id`,`account_id`)\
) ENGINE=InnoDB DEFAULT CHARSET=utf8;";

static const char* match_default_sqlite = "CREATE TABLE IF NOT EXISTS `match` (\
    `match_id` bigint(20) NOT NULL,\
    `duration` smallint(5),\
    `pre_game_duration` smallint(5),\
    `radiant_win` tinyint(3),\
    `start_time` int(10),\
    `match_seq_num` int(10),\
    `tower_status_radiant` smallint(5),\
    `tower_status_dire` smallint(5),\
    `barracks_status_radiant` tinyint(3),\
    `barracks_status_dire` tinyint(3),\
    `cluster` smallint(5),\
    `first_blood_time` smallint(5),\
    `lobby_type` tinyint(4),\
    `human_players` tinyint(3),\
    `leagueid` int(10),\
    `positive_votes` smallint(5),\
    `negative_votes` smallint(5),\
    `game_mode` tinyint(3),\
    `flags` int(10),\
    `engine` tinyint(3),\
    `radiant_score` smallint(5),\
    `dire_score` smallint(5),\
    PRIMARY KEY (`match_id`))";

static const char* match_players_default_sqlite = "CREATE TABLE IF NOT EXISTS `match_players` (\
    `account_id` int(10),\
    `match_id` bigint(20),\
    `player_slot` tinyint(3),\
    `hero_id` smallint(5),\
    `item_0` smallint(5),\
    `item_1` smallint(5),\
    `item_2` smallint(5),\
    `item_3` smallint(5),\
    `item_4` smallint(5),\
    `item_5` smallint(5),\
    `backpack_0` smallint(5),\
    `backpack_1` smallint(5),\
    `backpack_2` smallint(5),\
    `kills` smallint(5),\
    `deaths` smallint(5),\
    `assists` smallint(5),\
    `leaver_status` tinyint(3),\
    `last_hits` smallint(5),\
    `denies` smallint(5),\
    `gold_per_min` smallint(5),\
    `xp_per_min` smallint(5),\
    `level` tinyint(3),\
    `hero_damage` mediumint(8),\
    `tower_damage` mediumint(8),\
    `hero_healing` mediumint(8),\
    `gold` mediumint(8),\
    `gold_spent` mediumint(8),\
    `scaled_hero_damage` mediumint(8),\
    `scaled_tower_damage` mediumint(8),\
    `scaled_hero_healing` mediumint(8),\
    PRIMARY KEY (`match_id`,`account_id`))";

#endif // DEFAULT_SQL_H
