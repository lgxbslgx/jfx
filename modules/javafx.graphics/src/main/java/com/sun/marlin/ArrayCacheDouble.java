/*
 * Copyright (c) 2015, 2021, Oracle and/or its affiliates. All rights reserved.
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

package com.sun.marlin;

import static com.sun.marlin.ArrayCacheConst.ARRAY_SIZES;
import static com.sun.marlin.ArrayCacheConst.BUCKETS;
import static com.sun.marlin.ArrayCacheConst.MAX_ARRAY_SIZE;

import static com.sun.marlin.MarlinConst.DO_STATS;
import static com.sun.marlin.MarlinConst.DO_CHECKS;
import static com.sun.marlin.MarlinConst.DO_CLEAN_DIRTY;
import static com.sun.marlin.MarlinConst.DO_LOG_WIDEN_ARRAY;
import static com.sun.marlin.MarlinConst.DO_LOG_OVERSIZE;

import static com.sun.marlin.MarlinUtils.logInfo;
import static com.sun.marlin.MarlinUtils.logException;

import java.lang.ref.WeakReference;
import java.util.Arrays;

import com.sun.marlin.ArrayCacheConst.BucketStats;
import com.sun.marlin.ArrayCacheConst.CacheStats;

/*
 * Note that the ArrayCache[BYTE/INT/FLOAT/DOUBLE] files are nearly identical except
 * for a few type and name differences. Typically, the [BYTE]ArrayCache.java file
 * is edited manually and then [INT/FLOAT/DOUBLE]ArrayCache.java
 * files are generated with the following command lines:
 */

public final class ArrayCacheDouble {

    /* members */
    private final int bucketCapacity;
    private WeakReference<Bucket[]> refBuckets = null;
    final CacheStats stats;

    ArrayCacheDouble(final int bucketCapacity) {
        this.bucketCapacity = bucketCapacity;
        this.stats = (DO_STATS) ?
            new CacheStats("ArrayCacheDouble(Dirty)") : null;
    }

    Bucket getCacheBucket(final int length) {
        final int bucket = ArrayCacheConst.getBucket(length);
        return getBuckets()[bucket];
    }

    private Bucket[] getBuckets() {
        // resolve reference:
        Bucket[] buckets = (refBuckets != null) ? refBuckets.get() : null;

        // create a new buckets ?
        if (buckets == null) {
            buckets = new Bucket[BUCKETS];

            for (int i = 0; i < BUCKETS; i++) {
                buckets[i] = new Bucket(ARRAY_SIZES[i], bucketCapacity,
                        (DO_STATS) ? stats.bucketStats[i] : null);
            }

            // update weak reference:
            refBuckets = new WeakReference<>(buckets);
        }
        return buckets;
    }

    Reference createRef(final int initialSize) {
        return new Reference(this, initialSize);
    }

    static final class Reference {

        // initial array reference (direct access)
        final double[] initial;
        private final ArrayCacheDouble cache;

        Reference(final ArrayCacheDouble cache, final int initialSize) {
            this.cache = cache;
            this.initial = createArray(initialSize);
            if (DO_STATS) {
                cache.stats.totalInitial += initialSize;
            }
        }

        double[] getArray(final int length) {
            if (length <= MAX_ARRAY_SIZE) {
                return cache.getCacheBucket(length).getArray();
            }
            if (DO_STATS) {
                cache.stats.oversize++;
            }
            if (DO_LOG_OVERSIZE) {
                logInfo("ArrayCacheDouble(Dirty): "
                        + "getArray[oversize]: length=\t" + length);
            }
            return createArray(length);
        }

        double[] widenArray(final double[] array, final int usedSize,
                          final int needSize)
        {
            final int length = array.length;
            if (DO_CHECKS && length >= needSize) {
                return array;
            }
            if (DO_STATS) {
                cache.stats.resize++;
            }

            // maybe change bucket:
            // ensure getNewSize() > newSize:
            final double[] res = getArray(ArrayCacheConst.getNewSize(usedSize, needSize));

            // use wrapper to ensure proper copy:
            System.arraycopy(array, 0, res, 0, usedSize); // copy only used elements

            // maybe return current array:
            putArray(array, 0, usedSize); // ensure array is cleared

            if (DO_LOG_WIDEN_ARRAY) {
                logInfo("ArrayCacheDouble(Dirty): "
                        + "widenArray[" + res.length
                        + "]: usedSize=\t" + usedSize + "\tlength=\t" + length
                        + "\tneeded length=\t" + needSize);
            }
            return res;
        }

        boolean doCleanRef(final double[] array) {
            return DO_CLEAN_DIRTY || (array != initial);
        }

        double[] putArray(final double[] array)
        {
            // dirty array helper:
            return putArray(array, 0, array.length);
        }

        double[] putArray(final double[] array, final int fromIndex,
                        final int toIndex)
        {
            if (array.length <= MAX_ARRAY_SIZE) {
                if (DO_CLEAN_DIRTY && (toIndex != 0)) {
                    // clean-up array of dirty part[fromIndex; toIndex[
                    fill(array, fromIndex, toIndex, 0.0d);
                }
                // ensure to never store initial arrays in cache:
                if (array != initial) {
                    cache.getCacheBucket(array.length).putArray(array);
                }
            }
            return initial;
        }
    }

    static final class Bucket {

        private int tail = 0;
        private final int arraySize;
        private final double[][] arrays;
        private final BucketStats stats;

        Bucket(final int arraySize,
               final int capacity, final BucketStats stats)
        {
            this.arraySize = arraySize;
            this.stats = stats;
            this.arrays = new double[capacity][];
        }

        double[] getArray() {
            if (DO_STATS) {
                stats.getOp++;
            }
            // use cache:
            if (tail != 0) {
                final double[] array = arrays[--tail];
                arrays[tail] = null;
                return array;
            }
            if (DO_STATS) {
                stats.createOp++;
            }
            return createArray(arraySize);
        }

        void putArray(final double[] array)
        {
            if (DO_CHECKS && (array.length != arraySize)) {
                logInfo("ArrayCacheDouble(Dirty): "
                        + "bad length = " + array.length);
                return;
            }
            if (DO_STATS) {
                stats.returnOp++;
            }
            // fill cache:
            if (arrays.length > tail) {
                arrays[tail++] = array;

                if (DO_STATS) {
                    stats.updateMaxSize(tail);
                }
            } else if (DO_CHECKS) {
                logInfo("ArrayCacheDouble(Dirty): "
                        + "array capacity exceeded !");
            }
        }
    }

    static double[] createArray(final int length) {
        return new double[length];
    }

    static void fill(final double[] array, final int fromIndex,
                     final int toIndex, final double value)
    {
        // clear array data:
        Arrays.fill(array, fromIndex, toIndex, value);
        if (DO_CHECKS) {
            check(array, fromIndex, toIndex, value);
        }
    }

    static void check(final double[] array, final int fromIndex,
                      final int toIndex, final double value)
    {
        if (DO_CHECKS) {
            // check zero on full array:
            for (int i = 0; i < array.length; i++) {
                if (array[i] != value) {
                    logException("Invalid value at: " + i + " = " + array[i]
                            + " from: " + fromIndex + " to: " + toIndex + "\n"
                            + Arrays.toString(array), new Throwable());

                    // ensure array is correctly filled:
                    Arrays.fill(array, value);

                    return;
                }
            }
        }
    }
}
