/*
 * Copyright (c) 1999, 2013, Oracle and/or its affiliates. All rights reserved.
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
 *
 */

#ifndef SHARE_VM_CI_CIINSTANCEKLASS_HPP
#define SHARE_VM_CI_CIINSTANCEKLASS_HPP

#include "ci/ciConstantPoolCache.hpp"
#include "ci/ciFlags.hpp"
#include "ci/ciKlass.hpp"
#include "ci/ciSymbol.hpp"

// ciInstanceKlass
//
// This class represents a Klass* in the HotSpot virtual machine
// whose Klass part is an InstanceKlass.  It may or may not
// be loaded.
class ciInstanceKlass : public ciKlass {
  CI_PACKAGE_ACCESS
  friend class ciBytecodeStream;
  friend class ciEnv;
  friend class ciExceptionHandler;
  friend class ciMethod;
  friend class ciField;

private:
  jobject                _loader;
  jobject                _protection_domain;

  InstanceKlass::ClassState _init_state;           // state of class
  bool                   _is_shared;
  bool                   _has_finalizer;
  bool                   _has_subklass;
  bool                   _has_nonstatic_fields;
  bool                   _has_default_methods;

  ciFlags                _flags;
  jint                   _nonstatic_field_size;
  jint                   _nonstatic_oop_map_size;

  // Lazy fields get filled in only upon request.
  ciInstanceKlass*       _super;
  ciInstance*            _java_mirror;

  ciConstantPoolCache*   _field_cache;  // cached map index->field
  GrowableArray<ciField*>* _nonstatic_fields;

  // The possible values of the _implementor fall into following three cases:
  //   NULL: no implementor.
  //   A ciInstanceKlass that's not itself: one implementor.
  //   Itsef: more than one implementors.
  ciInstanceKlass*       _implementor;

  GrowableArray<ciField*>* _non_static_fields;

protected:
  ciInstanceKlass(KlassHandle h_k);
  ciInstanceKlass(ciSymbol* name, jobject loader, jobject protection_domain);

  InstanceKlass* get_instanceKlass() const {
    return (InstanceKlass*)get_Klass();
  }

  oop loader();
  jobject loader_handle();

  oop protection_domain();
  jobject protection_domain_handle();

  const char* type_string() { return "ciInstanceKlass"; }

  bool is_in_package_impl(const char* packagename, int len);

  void print_impl(outputStream* st);

  ciConstantPoolCache* field_cache();

  bool is_shared() { return _is_shared; }

  void compute_shared_init_state();
  bool compute_shared_has_subklass();
  int  compute_nonstatic_fields();
  GrowableArray<ciField*>* compute_nonstatic_fields_impl(GrowableArray<ciField*>* super_fields);

  // Update the init_state for shared klasses
  void update_if_shared(InstanceKlass::ClassState expected) {
    if (_is_shared && _init_state != expected) {
      if (is_loaded()) compute_shared_init_state();
    }
  }

public:
  // Has this klass been initialized?
  bool                   is_initialized() {
    update_if_shared(InstanceKlass::fully_initialized);
    return _init_state == InstanceKlass::fully_initialized;
  }
  // Is this klass being initialized?
  bool                   is_being_initialized() {
    update_if_shared(InstanceKlass::being_initialized);
    return _init_state == InstanceKlass::being_initialized;
  }
  // Has this klass been linked?
  bool                   is_linked() {
    update_if_shared(InstanceKlass::linked);
    return _init_state >= InstanceKlass::linked;
  }

  // General klass information.
  ciFlags                flags()          {
    assert(is_loaded(), "must be loaded");
    return _flags;
  }
  bool                   has_finalizer()  {
    assert(is_loaded(), "must be loaded");
    return _has_finalizer; }
  bool                   has_subklass()   {
    assert(is_loaded(), "must be loaded");
    if (_is_shared && !_has_subklass) {
      if (flags().is_final()) {
        return false;
      } else {
        return compute_shared_has_subklass();
      }
    }
    return _has_subklass;
  }
  jint                   size_helper()  {
    return (Klass::layout_helper_size_in_bytes(layout_helper())
            >> LogHeapWordSize);
  }
  jint                   nonstatic_field_size()  {
    assert(is_loaded(), "must be loaded");
    return _nonstatic_field_size; }
  jint                   has_nonstatic_fields()  {
    assert(is_loaded(), "must be loaded");
    return _has_nonstatic_fields; }
  jint                   nonstatic_oop_map_size()  {
    assert(is_loaded(), "must be loaded");
    return _nonstatic_oop_map_size; }
  ciInstanceKlass*       super();
  jint                   nof_implementors() {
    ciInstanceKlass* impl;
    assert(is_loaded(), "must be loaded");
    impl = implementor();
    if (impl == NULL) {
      return 0;
    } else if (impl != this) {
      return 1;
    } else {
      return 2;
    }
  }

  bool has_default_methods()  {
    assert(is_loaded(), "must be loaded");
    return _has_default_methods;
  }

  ciInstanceKlass* get_canonical_holder(int offset);
  ciField* get_field_by_offset(int field_offset, bool is_static);
  ciField* get_field_by_name(ciSymbol* name, ciSymbol* signature, bool is_static);

  GrowableArray<ciField*>* non_static_fields();

  // total number of nonstatic fields (including inherited):
  int nof_nonstatic_fields() {
    if (_nonstatic_fields == NULL)
      return compute_nonstatic_fields();
    else
      return _nonstatic_fields->length();
  }
  // nth nonstatic field (presented by ascending address)
  ciField* nonstatic_field_at(int i) {
    assert(_nonstatic_fields != NULL, "");
    return _nonstatic_fields->at(i);
  }

  ciInstanceKlass* unique_concrete_subklass();
  bool has_finalizable_subclass();

  bool contains_field_offset(int offset) {
    return instanceOopDesc::contains_field_offset(offset, nonstatic_field_size());
  }

  // Get the instance of java.lang.Class corresponding to
  // this klass.  This instance is used for locking of
  // synchronized static methods of this klass.
  ciInstance*            java_mirror();

  // Java access flags
  bool is_public      () { return flags().is_public(); }
  bool is_final       () { return flags().is_final(); }
  bool is_super       () { return flags().is_super(); }
  bool is_interface   () { return flags().is_interface(); }
  bool is_abstract    () { return flags().is_abstract(); }

  ciMethod* find_method(ciSymbol* name, ciSymbol* signature);
  // Note:  To find a method from name and type strings, use ciSymbol::make,
  // but consider adding to vmSymbols.hpp instead.

  bool is_leaf_type();
  ciInstanceKlass* implementor();

  // Is the defining class loader of this class the default loader?
  bool uses_default_loader() const;

  bool is_java_lang_Object() const;

  BasicType box_klass_type() const;
  bool is_box_klass() const;
  bool is_boxed_value_offset(int offset) const;

  // Is this klass in the given package?
  bool is_in_package(const char* packagename) {
    return is_in_package(packagename, (int) strlen(packagename));
  }
  bool is_in_package(const char* packagename, int len);

  // What kind of ciObject is this?
  bool is_instance_klass() const { return true; }
  bool is_java_klass() const     { return true; }

  virtual ciKlass* exact_klass() {
    if (is_loaded() && is_final() && !is_interface()) {
      return this;
    }
    return NULL;
  }

  // Dump the current state of this klass for compilation replay.
  virtual void dump_replay_data(outputStream* out);

#ifdef ASSERT
  bool debug_final_or_stable_field_at(int offset);
#endif
};

#endif // SHARE_VM_CI_CIINSTANCEKLASS_HPP
