/**
 * brillouin zone tool
 * @author Tobias Weber <tweber@ill.fr>
 * @date Maz-2022
 * @license GPLv3, see 'LICENSE' file
 *
 * ----------------------------------------------------------------------------
 * mag-core (part of the Takin software suite)
 * Copyright (C) 2018-2024  Tobias WEBER (Institut Laue-Langevin (ILL),
 *                          Grenoble, France).
 * "misc" project
 * Copyright (C) 2017-2021  Tobias WEBER (privately developed).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------------
 */

#ifndef __BZTOOL_GLOBALS_H__
#define __BZTOOL_GLOBALS_H__


#include <boost/math/quaternion.hpp>
namespace math = boost::math;

#include "tlibs2/libs/maths.h"
#include "libs/defs.h"


using t_vec_bz = tl2::vec<t_real, std::vector>;
using t_mat_bz = tl2::mat<t_real, std::vector>;
using t_quat_bz = math::quaternion<t_real>;

extern t_real g_eps_bz;
extern int g_prec_bz;
extern int g_prec_gui_bz;

extern void set_eps_bz(t_real eps, int prec = -1);


#ifndef DONT_USE_QT
	#include "tlibs2/libs/qt/gl.h"

	using t_real_gl = tl2::t_real_gl;
	using t_vec2_gl = tl2::t_vec2_gl;
	using t_vec3_gl = tl2::t_vec3_gl;
	using t_vec_gl = tl2::t_vec_gl;
	using t_mat_gl = tl2::t_mat_gl;
#endif

#endif
