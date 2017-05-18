CREATE TABLE scripts (
    id SERIAL PRIMARY KEY,
    "name" citext NOT NULL,
    script text NOT NULL
);

CREATE TABLE script_triggers (
    id SERIAL PRIMARY KEY,
    trigger_type citext NOT NULL,
    script_id INT NOT NULL,
    global BOOLEAN NOT NULL
);

CREATE TABLE script_zones (
    id SERIAL PRIMARY KEY,
    x BIGINT NOT NULL,
    y BIGINT NOT NULL,
    width BIGINT NOT NULL,
    height BIGINT NOT NULL
);

CREATE TABLE stats (
    id SERIAL PRIMARY KEY,
    "name" citext NOT NULL
);

CREATE TABLE locations (
    id BIGSERIAL PRIMARY KEY,
    map_id BIGINT NOT NULL,
    x INTEGER NOT NULL,
    y INTEGER NOT NULL
);

CREATE TABLE maps (
    id SERIAL PRIMARY KEY,
    "name" citext NOT NULL,
    data text NOT NULL
);

CREATE TABLE map_tiles (
    id BIGSERIAL PRIMARY KEY,
    map_id INT NOT NULL,
    x INT NOT NULL,
    y INT NOT NULL
);

CREATE TABLE players (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL,
    location_id BIGINT NOT NULL,
    "name" citext NOT NULL
);

CREATE TABLE player_stats (
    id BIGSERIAL PRIMARY KEY,
    player_id BIGINT NOT NULL,
    stat_id INT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE player_growth_stats (
    id BIGSERIAL PRIMARY KEY,
    player_id BIGINT NOT NULL,
    stat_id INT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE items (
    id BIGSERIAL PRIMARY KEY,
    player_id BIGINT NULL,
    npc_id INT NULL,
    location_id BIGINT NULL,
    "name" citext NOT NULL
);

CREATE TABLE item_stats (
    id BIGSERIAL PRIMARY KEY,
    item_id BIGINT NOT NULL,
    stat_id INT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE item_growth_stats (
    id BIGSERIAL PRIMARY KEY,
    item_id BIGINT NOT NULL,
    stat_id INT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE npcs (
    id SERIAL PRIMARY KEY,
    location_id BIGINT NOT NULL,
    "name" citext NOT NULL
);

CREATE TABLE npc_stats (
    id BIGSERIAL PRIMARY KEY,
    npc_id INT NOT NULL,
    stat_id INT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE npc_growth_stats (
    id BIGSERIAL PRIMARY KEY,
    npc_id INT NOT NULL,
    stat_id INT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE objects (
    id BIGSERIAL PRIMARY KEY,
    location_id BIGINT NOT NULL,
    "name" citext NOT NULL
);

CREATE TABLE settings (
    "name" citext NOT NULL,
    value text not null
);

ALTER TABLE script_triggers ADD CONSTRAINT "script_triggers_scripts_id_fkey" FOREIGN KEY (script_id) REFERENCES scripts(id);
ALTER TABLE locations ADD CONSTRAINT "locations_maps_id_fkey" FOREIGN KEY (map_id) REFERENCES maps(id);
ALTER TABLE map_tiles ADD CONSTRAINT "map_tiles_maps_id_fkey" FOREIGN KEY (map_id) REFERENCES maps(id);
ALTER TABLE players ADD CONSTRAINT "players_locations_id_fkey" FOREIGN KEY (location_id) REFERENCES locations(id);
ALTER TABLE player_stats ADD CONSTRAINT "player_stats_players_id_fkey" FOREIGN KEY (player_id) REFERENCES players(id);
ALTER TABLE player_stats ADD CONSTRAINT "player_stats_stats_id_fkey" FOREIGN KEY (stat_id) REFERENCES stats(id);
ALTER TABLE player_growth_stats ADD CONSTRAINT "player_growth_stats_players_id_fkey" FOREIGN KEY (player_id) REFERENCES players(id);
ALTER TABLE player_growth_stats ADD CONSTRAINT "player_growth_stats_stats_id_fkey" FOREIGN KEY (stat_id) REFERENCES stats(id);
ALTER TABLE items ADD CONSTRAINT "items_players_id_fkey" FOREIGN KEY (player_id) REFERENCES players(id);
ALTER TABLE items ADD CONSTRAINT "items_npcs_id_fkey" FOREIGN KEY (npc_id) REFERENCES npcs(id);
ALTER TABLE items ADD CONSTRAINT "items_locations_id_fkey" FOREIGN KEY (location_id) REFERENCES locations(id);
ALTER TABLE item_stats ADD CONSTRAINT "item_stats_items_id_fkey" FOREIGN KEY (item_id) REFERENCES items(id);
ALTER TABLE item_stats ADD CONSTRAINT "item_stats_stats_id_fkey" FOREIGN KEY (stat_id) REFERENCES stats(id);
ALTER TABLE item_growth_stats ADD CONSTRAINT "item_growth_stats_players_id_fkey" FOREIGN KEY (item_id) REFERENCES items(id);
ALTER TABLE item_growth_stats ADD CONSTRAINT "item_growth_stats_stats_id_fkey" FOREIGN KEY (stat_id) REFERENCES stats(id);
ALTER TABLE npcs ADD CONSTRAINT "npcs_location_id_fkey" FOREIGN KEY (location_id) REFERENCES locations(id);
ALTER TABLE npc_stats ADD CONSTRAINT "npc_stats_items_id_fkey" FOREIGN KEY (npc_id) REFERENCES npcs(id);
ALTER TABLE npc_stats ADD CONSTRAINT "npc_stats_stats_id_fkey" FOREIGN KEY (stat_id) REFERENCES stats(id);
ALTER TABLE npc_growth_stats ADD CONSTRAINT "npc_growth_stats_players_id_fkey" FOREIGN KEY (npc_id) REFERENCES npcs(id);
ALTER TABLE npc_growth_stats ADD CONSTRAINT "npc_growth_stats_stats_id_fkey" FOREIGN KEY (stat_id) REFERENCES stats(id);
ALTER TABLE objects ADD CONSTRAINT "objects_locations_id_fkey" FOREIGN KEY (location_id) REFERENCES locations(id);
