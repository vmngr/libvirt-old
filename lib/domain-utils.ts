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
import { NodeMemoryStatsStruct } from ".";

 export function prettifyMemoryUsage(memoryStats: any) {
    const pretty = _.mapValues(memoryStats, (value, key) => {
        if (key == "last_update") { return value; }
        return bytes(value * 1024, { unit: "MB"});
    })
    pretty.percentage =  Number((memoryStats.used / memoryStats.available * 100).toFixed(2));
    return pretty;
 }

 export function prettifyNodeMemoryUsage(memoryStats: NodeMemoryStatsStruct) {
     const pretty = _.mapValues(memoryStats, (value, key) => {
         return bytes(value * 1024, { unit: "GB" });
     })
     return pretty;
 }

 export function prettifyCpuUsage(prev: any, current: any) {
    let sum: any = {};
    _.keys(prev).map((keyName: string, index: number)=>{
        if (keyName == "total") return;
        const key_diff = current[keyName] - prev[keyName];
        const total_time_diff = current['total'] - prev['total'];
        sum = {
            ...sum,
            [keyName]: key_diff / total_time_diff * 100
        }
     });
     sum.usage = sum.user + sum.system;
     return sum;

 }