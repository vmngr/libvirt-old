/**
 * Copyright 2019 Leon Rinkel <leon@rinkel.me> and vmngr/libvirt contributers.
 *
 * This file is part of the vmngr/libvirt project and is subject to the MIT
 * license as in the LICENSE file in the project root.
 *
 * @brief Contains actual libvirt bindings and related declarations.
 */

import _ from "lodash";
import bytes from "bytes";

 export function prettifyMemoryUsage(memoryStats: any) {
    const pretty = _.mapValues(memoryStats, (value, key) => {
        if (key == "last_update") { return value; }
        return bytes(value * 1024, { unit: "GB"});
    })
    memoryStats.percentage =  Number((memoryStats.used / memoryStats.actual * 100).toFixed(2));
    return pretty;
 }

 export function prettifyNodeMemoryUsage(memoryStats: any) {
     const pretty = _.mapValues(memoryStats, (value, key) => {
         return bytes(value * 1024, { unit: "GB" });
     })
 }