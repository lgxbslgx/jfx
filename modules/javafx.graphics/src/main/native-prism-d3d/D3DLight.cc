/*
 * Copyright (c) 2013, 2022, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include <iostream>
#include "D3DLight.h"
#include "D3DContext.h"
using std::cout;
using std::endl;

// Destructor definition

D3DLight::~D3DLight() {
}

D3DLight::D3DLight() :
    color(),
    position(),
    w(0),
    attenuation(),
    maxRange(0),
    direction(),
    innerAngle(0),
    outerAngle(0),
    falloff(0)
    {}

bool D3DLight::isPointLight() {
    return falloff == 0 && outerAngle == 180 && attenuation[3] > 0.5;
}

bool D3DLight::isDirectionalLight() {
    // testing if w is 0 or 1 using <0.5 since equality check for floating points might not work well
    return attenuation[3] < 0.5;
}

void D3DLight::setColor(float r, float g, float b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void D3DLight::setPosition(float x, float y, float z) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
}
