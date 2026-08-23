/*
 * Copyright (C) 2025 Tencent.
 * BQLOG is licensed under the Apache License, Version 2.0.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
import { test_base } from "./test_base.ts";
import { test_result } from "./test_result.ts";
import { bq } from "@pippocao/bqlog";
import { test_category_log } from "./test_category_log.ts";

export class test_log_category extends test_base {
    constructor(name: string) {
        super(name);
    }

    public async test(): Promise<test_result> {
        const result = new test_result();

        // Test 1-3: Category output and hierarchy verification using snapshot
        const cat_log = test_category_log.create_log("cat_test_1",
            "appenders_config.ConsoleAppender.type=console\n"
            + "appenders_config.ConsoleAppender.time_zone=localtime\n"
            + "appenders_config.ConsoleAppender.levels=[all]\n"
            + "log.thread_mode=sync\n"
            + "snapshot.buffer_size=65536\n"
            + "snapshot.levels=[all]");

        const snapshot_before = cat_log.take_snapshot("gmt");

        // Test 1: Category output verification
        cat_log.info(cat_log.cat.ModuleA.SystemA, "Hello Category");
        const snapshot_1 = cat_log.take_snapshot("gmt");
        result.add_result(
            snapshot_1 !== snapshot_before
            && snapshot_1.includes("[ModuleA.SystemA]")
            && snapshot_1.endsWith("Hello Category\n"),
            "category output test");

        // Test 2: Deep category hierarchy verification
        cat_log.error(cat_log.cat.ModuleA.SystemA.ClassA, "Deep Category");
        const snapshot_2 = cat_log.take_snapshot("gmt");
        result.add_result(
            snapshot_2 !== snapshot_1
            && snapshot_2.includes("[ModuleA.SystemA.ClassA]")
            && snapshot_2.endsWith("Deep Category\n"),
            "deep category test");

        // Test 3: Category with format parameters
        cat_log.info(cat_log.cat.ModuleB, "Param test: {}, {}", "hello", 42);
        const snapshot_3 = cat_log.take_snapshot("gmt");
        result.add_result(
            snapshot_3 !== snapshot_2
            && snapshot_3.includes("[ModuleB]")
            && snapshot_3.endsWith("Param test: hello, 42\n"),
            "category param test");

        // Test 4-6: Category mask filtering using snapshot
        const masked_log = test_category_log.create_log("cat_test_mask",
            "appenders_config.ConsoleAppender.type=console\n"
            + "appenders_config.ConsoleAppender.time_zone=localtime\n"
            + "appenders_config.ConsoleAppender.levels=[all]\n"
            + "log.thread_mode=sync\n"
            + "log.categories_mask=[ModuleA.SystemA.ClassA,ModuleB]\n"
            + "snapshot.buffer_size=65536\n"
            + "snapshot.levels=[all]\n"
            + "snapshot.categories_mask=[ModuleA.SystemA.ClassA,ModuleB]");

        const snapshot_mask_before = masked_log.take_snapshot("gmt");

        // Test 4: This should be filtered (ModuleA.SystemA is NOT in mask)
        masked_log.info(masked_log.cat.ModuleA.SystemA, "should be filtered");
        const snapshot_mask_filtered = masked_log.take_snapshot("gmt");
        result.add_result(
            snapshot_mask_filtered === snapshot_mask_before,
            "category mask filter test");

        // Test 5: This should pass (ModuleA.SystemA.ClassA IS in mask)
        masked_log.info(masked_log.cat.ModuleA.SystemA.ClassA, "should pass");
        const snapshot_mask_pass = masked_log.take_snapshot("gmt");
        result.add_result(
            snapshot_mask_pass !== snapshot_mask_filtered
            && snapshot_mask_pass.includes("should pass"),
            "category mask pass test");

        // Test 6: This should pass (ModuleB IS in mask)
        masked_log.info(masked_log.cat.ModuleB, "ModuleB pass");
        const snapshot_mask_moduleb = masked_log.take_snapshot("gmt");
        result.add_result(
            snapshot_mask_moduleb !== snapshot_mask_pass
            && snapshot_mask_moduleb.includes("ModuleB pass"),
            "category mask ModuleB test");

        return result;
    }
}
