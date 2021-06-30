/*
 * Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
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


/*
 * @test
 *
 * @summary converted from VM Testbase nsk/jdi/ClassType/invokeMethod/invokemethod007.
 * VM Testbase keywords: [jpda, jdi]
 * VM Testbase readme:
 * DESCRIPTION:
 *   The test checks up the method com.sun.jdi.ClassType.invokeMethod()
 *   Test checks up the following assertion:
 *       ...all threads in the target VM are resumed while the method is
 *       being invoked if they were previously suspended by an event or by
 *       VirtualMachine.suspend() or ThreadReference.suspend().
 *   This checking is implemented for three cases:
 *       - when the tested thread is suspended by ThreadReference.suspend();
 *       - when the tested thread is suspended by VirtualMachine.suspend();
 *       - when the tested thread is suspended by an event.
 * COMMENTS:
 *   4786806 TEST_BUG: debugee does not relinquishes lock after notifying
 *   Test fixed according to test bug:
 *     4798088 TEST_BUG: setBreakpoint() method depends of the locations implementation
 *     - using standard Debugee.makeBreakpoint() method for setting breakpoint
 *
 * @library /vmTestbase
 *          /test/lib
 * @run driver jdk.test.lib.FileInstaller . .
 * @build nsk.jdi.ClassType.invokeMethod.invokemethod007
 *        nsk.jdi.ClassType.invokeMethod.invokemethod007a
 * @run main/othervm PropertyResolvingWrapper
 *      nsk.jdi.ClassType.invokeMethod.invokemethod007
 *      -verbose
 *      -arch=${os.family}-${os.simpleArch}
 *      -waittime=5
 *      -debugee.vmkind=java
 *      -transport.address=dynamic
 *      "-debugee.vmkeys=${test.vm.opts} ${test.java.opts} -Djava.library.path=${java.library.path}"
 */

