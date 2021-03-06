/*
   Copyright (C) 2003 - 2014 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/** @file */

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

class config;

#include "map_location.hpp"
#include "terrain_translation.hpp"
#include "terrain_type_data.hpp"

#include <boost/shared_ptr.hpp>

//class terrain_type_data; Can't forward declare because of enum

/**
 * Encapsulates the map of the game.
 *
 * Although the game is hexagonal, the map is stored as a grid.
 * Each type of terrain is represented by a multiletter terrain code.
 * @todo Update for new map-format.
 */
class gamemap
{
public:

	/* Get info from the terrain_type_data object about the terrain at a location */
	const t_translation::t_list& underlying_mvt_terrain(const map_location& loc) const;
	const t_translation::t_list& underlying_def_terrain(const map_location& loc) const;
	const t_translation::t_list& underlying_union_terrain(const map_location& loc) const;
	std::string get_terrain_string(const map_location& loc) const;
	std::string get_terrain_editor_string(const map_location& loc) const;

	bool is_village(const map_location& loc) const;
	int gives_healing(const map_location& loc) const;
	bool is_castle(const map_location& loc) const;
	bool is_keep(const map_location& loc) const;

	/* The above wrappers, but which takes a terrain. This is the old syntax, preserved for brevity in certain cases. */
	const t_translation::t_list& underlying_mvt_terrain(const t_translation::t_terrain & terrain) const;
	const t_translation::t_list& underlying_def_terrain(const t_translation::t_terrain & terrain) const;
	const t_translation::t_list& underlying_union_terrain(const t_translation::t_terrain & terrain) const;
	std::string get_terrain_string(const t_translation::t_terrain& terrain) const;
	std::string get_terrain_editor_string(const t_translation::t_terrain& terrain) const;
	std::string get_underlying_terrain_string(const t_translation::t_terrain& terrain) const;

	bool is_village(const t_translation::t_terrain & terrain) const;
	int gives_healing(const t_translation::t_terrain & terrain) const;
	bool is_castle(const t_translation::t_terrain & terrain) const;
	bool is_keep(const t_translation::t_terrain & terrain) const;

	// Also expose this for the same reason:
	const terrain_type& get_terrain_info(const t_translation::t_terrain & terrain) const;

	/* Get the underlying terrain_type_data object. */
	const tdata_cache & tdata() const { return tdata_; }

	enum tborder {
		NO_BORDER = 0,
		SINGLE_TILE_BORDER
		};

	enum tusage {
		IS_MAP,
		IS_MASK
		};

	/**
	 * Loads a map, with the given terrain configuration.
	 *
	 * Data should be a series of lines, with each character representing one
	 * hex on the map.  Starting locations are represented by numbers.
	 *
	 * @param tdata the terrain data
	 * @param data the map data to load.
	 */
	gamemap(const tdata_cache &tdata, const std::string &data); //throw(incorrect_map_format_error)


	/**
	 * Loads a map, from the [map] wml config in @a level.
	 *
	 * Data should be a series of lines, with each character representing one
	 * hex on the map.  Starting locations are represented by numbers
	 *
	 * @param tdata the terrain data
	 * @param level the scenario config to load from.
	 */
	gamemap(const tdata_cache &tdata, const config &level); //throw(incorrect_map_format_error)

	virtual ~gamemap();

	void read(const std::string& data, const bool allow_invalid = true, const int border_size = 1, const std::string usage = "map");

	std::string write() const;

	/** Overlays another map onto this one at the given position. */
	void overlay(const gamemap& m, const config& rules, int x=0, int y=0, bool border=false);

	/** Effective map width. */
	int w() const { return w_; }

	/** Effective map height. */
	int h() const { return h_; }

	/** Size of the map border. */
	int border_size() const { return border_size_; }

	/** Real width of the map, including borders. */
	int total_width()  const { return total_width_; }

	/** Real height of the map, including borders */
	int total_height() const { return total_height_; }

	const t_translation::t_terrain operator[](const map_location& loc) const
		{ return tiles_[loc.x + border_size_][loc.y + border_size_]; }

	/**
	 * Looks up terrain at a particular location.
	 *
	 * Hexes off the map may be looked up, and their 'emulated' terrain will
	 * also be returned.  This allows proper drawing of the edges of the map.
	 */
	t_translation::t_terrain get_terrain(const map_location& loc) const;

	/** Writes the terrain at loc to cfg. */
	void write_terrain(const map_location &loc, config& cfg) const;


	/** Manipulate starting positions of the different sides. */
	const map_location& starting_position(int side) const;
	int is_starting_position(const map_location& loc) const;
	int num_valid_starting_positions() const;

	void set_starting_position(int side, const map_location& loc);

	/**
	 * Tell if a location is on the map.
	 *
	 * Should be called before indexing using [].
	 * @todo inline for performance? -- Ilor
	 */
	bool on_board(const map_location& loc) const;
	bool on_board_with_border(const map_location& loc) const;

	/** Tell if the map is of 0 size. */
	bool empty() const
	{
		return w_ == 0 || h_ == 0;
	}

	/** Return a list of the locations of villages on the map. */
	const std::vector<map_location>& villages() const { return villages_; }

	/** Shortcut to get_terrain_info(get_terrain(loc)). */
	const terrain_type& get_terrain_info(const map_location &loc) const;

	/** Gets the list of terrains. */
	const t_translation::t_list& get_terrain_list() const;

	/**
	 * Clobbers over the terrain at location 'loc', with the given terrain.
	 * Uses mode and replace_if_failed like merge_terrains().
	 */
	void set_terrain(const map_location& loc, const t_translation::t_terrain & terrain, const terrain_type_data::tmerge_mode mode=terrain_type_data::BOTH, bool replace_if_failed = false);

	/**
	 * Returns a list of the frequencies of different terrain types on the map,
	 * with terrain nearer the center getting weighted higher.
	 */
	const std::map<t_translation::t_terrain, size_t>& get_weighted_terrain_frequencies() const;

	/**
	 * Remove the cached border terrain at loc.
	 *
	 * Needed by the editor to make tiles at the border update correctly when
	 * drawing other tiles.
	 */
	void remove_from_border_cache(const map_location &loc)
		{ borderCache_.erase(loc); }

	/**
	 * Maximum number of players supported.
	 *
	 * Warning: when you increase this, you need to add
	 * more definitions to the team_colors.cfg file.
	 */
	enum { MAX_PLAYERS = 9 };

	/** Returns the usage of the map. */
	tusage get_usage() const { return usage_; }

	/**
	 * The default map header, needed for maps created with
	 * terrain_translation::write_game_map().
	 */
	static const std::string default_map_header;

	/** The default border style for a map. */
	static const tborder default_border;

	/** Parses ranges of locations into a vector of locations, using this map's dimensions as bounds. */
	std::vector<map_location> parse_location_range(const std::string& xvals,
	const std::string &yvals, bool with_border = false) const;


protected:
	t_translation::t_map tiles_;

	/**
	 * The size of the starting positions array is MAX_PLAYERS + 1,
	 * because the positions themselves are numbered from 1.
	 */
	map_location startingPositions_[MAX_PLAYERS+1];

	/**
	 * Clears the border cache, needed for the editor
	 */
	void clear_border_cache() { borderCache_.clear(); }

private:

	void set_usage(const std::string& usage);

	/**
	 * Reads the header of a map which is saved in the deprecated map_data format.
	 *
	 * @param data		          The mapdata to load.
	 */
	int read_header(const std::string& data);

	int num_starting_positions() const
		{ return sizeof(startingPositions_)/sizeof(*startingPositions_); }

	/** Allows lookup of terrain at a particular location. */
	const t_translation::t_list operator[](int index) const
		{ return tiles_[index + border_size_]; }


	tdata_cache tdata_;
	std::vector<map_location> villages_;

	mutable std::map<map_location, t_translation::t_terrain> borderCache_;
	mutable std::map<t_translation::t_terrain, size_t> terrainFrequencyCache_;

protected:
	/** Sizes of the map area. */
	int w_;
	int h_;

	/** Sizes of the map including the borders. */
	int total_width_;
	int total_height_;

private:
	/** The size of the border around the map. */
	int border_size_;

	/** The kind of map is being loaded. */
	tusage usage_;
};

#endif
