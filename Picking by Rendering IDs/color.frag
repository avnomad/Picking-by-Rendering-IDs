//	Copyright (C) 2010-2012 Vaptistis Anogeianakis <nomad@cornercase.gr>
/*
 *	This file is part of Picking by Rendering IDs.
 *
 *	Picking by Rendering IDs is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	Picking by Rendering IDs is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with Picking by Rendering IDs.  If not, see <http://www.gnu.org/licenses/>.
 */

#version 140

uniform vec3 color;

void main()
{
	gl_FragColor = vec4(color,1.0);
} // end main
