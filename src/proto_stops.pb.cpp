// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proto_stops.proto

#include "../include/proto/proto_stops.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)

namespace protobuf_proto_5fstop_2eproto {
extern PROTOBUF_INTERNAL_EXPORT_protobuf_proto_5fstop_2eproto ::google::protobuf::internal::SCCInfo<1> scc_info_ProtoStop;
}  // namespace protobuf_proto_5fstop_2eproto
namespace protobuf_proto_5fstops_2eproto {
extern PROTOBUF_INTERNAL_EXPORT_protobuf_proto_5fstops_2eproto ::google::protobuf::internal::SCCInfo<0> scc_info_ProtoStops_IndexesByIdEntry_DoNotUse;
}  // namespace protobuf_proto_5fstops_2eproto
class ProtoStops_IndexesByIdEntry_DoNotUseDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<ProtoStops_IndexesByIdEntry_DoNotUse>
      _instance;
} _ProtoStops_IndexesByIdEntry_DoNotUse_default_instance_;
class ProtoStopsDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<ProtoStops>
      _instance;
} _ProtoStops_default_instance_;
namespace protobuf_proto_5fstops_2eproto {
static void InitDefaultsProtoStops_IndexesByIdEntry_DoNotUse() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_ProtoStops_IndexesByIdEntry_DoNotUse_default_instance_;
    new (ptr) ::ProtoStops_IndexesByIdEntry_DoNotUse();
  }
  ::ProtoStops_IndexesByIdEntry_DoNotUse::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_ProtoStops_IndexesByIdEntry_DoNotUse =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsProtoStops_IndexesByIdEntry_DoNotUse}, {}};

static void InitDefaultsProtoStops() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_ProtoStops_default_instance_;
    new (ptr) ::ProtoStops();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::ProtoStops::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<2> scc_info_ProtoStops =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 2, InitDefaultsProtoStops}, {
      &protobuf_proto_5fstop_2eproto::scc_info_ProtoStop.base,
      &protobuf_proto_5fstops_2eproto::scc_info_ProtoStops_IndexesByIdEntry_DoNotUse.base,}};

void InitDefaults() {
  ::google::protobuf::internal::InitSCC(&scc_info_ProtoStops_IndexesByIdEntry_DoNotUse.base);
  ::google::protobuf::internal::InitSCC(&scc_info_ProtoStops.base);
}

::google::protobuf::Metadata file_level_metadata[2];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::ProtoStops_IndexesByIdEntry_DoNotUse, _has_bits_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::ProtoStops_IndexesByIdEntry_DoNotUse, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::ProtoStops_IndexesByIdEntry_DoNotUse, key_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::ProtoStops_IndexesByIdEntry_DoNotUse, value_),
  0,
  1,
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::ProtoStops, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::ProtoStops, stops_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::ProtoStops, indexes_by_id_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 7, sizeof(::ProtoStops_IndexesByIdEntry_DoNotUse)},
  { 9, -1, sizeof(::ProtoStops)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::_ProtoStops_IndexesByIdEntry_DoNotUse_default_instance_),
  reinterpret_cast<const ::google::protobuf::Message*>(&::_ProtoStops_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  AssignDescriptors(
      "proto_stops.proto", schemas, file_default_instances, TableStruct::offsets,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 2);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\021proto_stops.proto\032\020proto_stop.proto\"\220\001"
      "\n\nProtoStops\022\031\n\005stops\030\001 \003(\0132\n.ProtoStop\022"
      "3\n\rindexes_by_id\030\002 \003(\0132\034.ProtoStops.Inde"
      "xesByIdEntry\0322\n\020IndexesByIdEntry\022\013\n\003key\030"
      "\001 \001(\003\022\r\n\005value\030\002 \001(\003:\0028\001b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 192);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "proto_stops.proto", &protobuf_RegisterTypes);
  ::protobuf_proto_5fstop_2eproto::AddDescriptors();
}

void AddDescriptors() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_proto_5fstops_2eproto

// ===================================================================

ProtoStops_IndexesByIdEntry_DoNotUse::ProtoStops_IndexesByIdEntry_DoNotUse() {}
ProtoStops_IndexesByIdEntry_DoNotUse::ProtoStops_IndexesByIdEntry_DoNotUse(::google::protobuf::Arena* arena) : SuperType(arena) {}
void ProtoStops_IndexesByIdEntry_DoNotUse::MergeFrom(const ProtoStops_IndexesByIdEntry_DoNotUse& other) {
  MergeFromInternal(other);
}
::google::protobuf::Metadata ProtoStops_IndexesByIdEntry_DoNotUse::GetMetadata() const {
  ::protobuf_proto_5fstops_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_proto_5fstops_2eproto::file_level_metadata[0];
}
void ProtoStops_IndexesByIdEntry_DoNotUse::MergeFrom(
    const ::google::protobuf::Message& other) {
  ::google::protobuf::Message::MergeFrom(other);
}


// ===================================================================

void ProtoStops::InitAsDefaultInstance() {
}
void ProtoStops::clear_stops() {
  stops_.Clear();
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int ProtoStops::kStopsFieldNumber;
const int ProtoStops::kIndexesByIdFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

ProtoStops::ProtoStops()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  ::google::protobuf::internal::InitSCC(
      &protobuf_proto_5fstops_2eproto::scc_info_ProtoStops.base);
  SharedCtor();
  // @@protoc_insertion_point(constructor:ProtoStops)
}
ProtoStops::ProtoStops(const ProtoStops& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      stops_(from.stops_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  indexes_by_id_.MergeFrom(from.indexes_by_id_);
  // @@protoc_insertion_point(copy_constructor:ProtoStops)
}

void ProtoStops::SharedCtor() {
}

ProtoStops::~ProtoStops() {
  // @@protoc_insertion_point(destructor:ProtoStops)
  SharedDtor();
}

void ProtoStops::SharedDtor() {
}

void ProtoStops::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const ::google::protobuf::Descriptor* ProtoStops::descriptor() {
  ::protobuf_proto_5fstops_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_proto_5fstops_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const ProtoStops& ProtoStops::default_instance() {
  ::google::protobuf::internal::InitSCC(&protobuf_proto_5fstops_2eproto::scc_info_ProtoStops.base);
  return *internal_default_instance();
}


void ProtoStops::Clear() {
// @@protoc_insertion_point(message_clear_start:ProtoStops)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  stops_.Clear();
  indexes_by_id_.Clear();
  _internal_metadata_.Clear();
}

bool ProtoStops::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:ProtoStops)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .ProtoStop stops = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessage(
                input, add_stops()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // map<int64, int64> indexes_by_id = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u /* 18 & 0xFF */)) {
          ProtoStops_IndexesByIdEntry_DoNotUse::Parser< ::google::protobuf::internal::MapField<
              ProtoStops_IndexesByIdEntry_DoNotUse,
              ::google::protobuf::int64, ::google::protobuf::int64,
              ::google::protobuf::internal::WireFormatLite::TYPE_INT64,
              ::google::protobuf::internal::WireFormatLite::TYPE_INT64,
              0 >,
            ::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 > > parser(&indexes_by_id_);
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
              input, &parser));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:ProtoStops)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:ProtoStops)
  return false;
#undef DO_
}

void ProtoStops::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:ProtoStops)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated .ProtoStop stops = 1;
  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->stops_size()); i < n; i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1,
      this->stops(static_cast<int>(i)),
      output);
  }

  // map<int64, int64> indexes_by_id = 2;
  if (!this->indexes_by_id().empty()) {
    typedef ::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 >::const_pointer
        ConstPtr;
    typedef ::google::protobuf::internal::SortItem< ::google::protobuf::int64, ConstPtr > SortItem;
    typedef ::google::protobuf::internal::CompareByFirstField<SortItem> Less;

    if (output->IsSerializationDeterministic() &&
        this->indexes_by_id().size() > 1) {
      ::std::unique_ptr<SortItem[]> items(
          new SortItem[this->indexes_by_id().size()]);
      typedef ::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 >::size_type size_type;
      size_type n = 0;
      for (::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 >::const_iterator
          it = this->indexes_by_id().begin();
          it != this->indexes_by_id().end(); ++it, ++n) {
        items[static_cast<ptrdiff_t>(n)] = SortItem(&*it);
      }
      ::std::sort(&items[0], &items[static_cast<ptrdiff_t>(n)], Less());
      ::std::unique_ptr<ProtoStops_IndexesByIdEntry_DoNotUse> entry;
      for (size_type i = 0; i < n; i++) {
        entry.reset(indexes_by_id_.NewEntryWrapper(
            items[static_cast<ptrdiff_t>(i)].second->first, items[static_cast<ptrdiff_t>(i)].second->second));
        ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
            2, *entry, output);
      }
    } else {
      ::std::unique_ptr<ProtoStops_IndexesByIdEntry_DoNotUse> entry;
      for (::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 >::const_iterator
          it = this->indexes_by_id().begin();
          it != this->indexes_by_id().end(); ++it) {
        entry.reset(indexes_by_id_.NewEntryWrapper(
            it->first, it->second));
        ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
            2, *entry, output);
      }
    }
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:ProtoStops)
}

::google::protobuf::uint8* ProtoStops::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:ProtoStops)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated .ProtoStop stops = 1;
  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->stops_size()); i < n; i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageToArray(
        1, this->stops(static_cast<int>(i)), deterministic, target);
  }

  // map<int64, int64> indexes_by_id = 2;
  if (!this->indexes_by_id().empty()) {
    typedef ::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 >::const_pointer
        ConstPtr;
    typedef ::google::protobuf::internal::SortItem< ::google::protobuf::int64, ConstPtr > SortItem;
    typedef ::google::protobuf::internal::CompareByFirstField<SortItem> Less;

    if (deterministic &&
        this->indexes_by_id().size() > 1) {
      ::std::unique_ptr<SortItem[]> items(
          new SortItem[this->indexes_by_id().size()]);
      typedef ::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 >::size_type size_type;
      size_type n = 0;
      for (::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 >::const_iterator
          it = this->indexes_by_id().begin();
          it != this->indexes_by_id().end(); ++it, ++n) {
        items[static_cast<ptrdiff_t>(n)] = SortItem(&*it);
      }
      ::std::sort(&items[0], &items[static_cast<ptrdiff_t>(n)], Less());
      ::std::unique_ptr<ProtoStops_IndexesByIdEntry_DoNotUse> entry;
      for (size_type i = 0; i < n; i++) {
        entry.reset(indexes_by_id_.NewEntryWrapper(
            items[static_cast<ptrdiff_t>(i)].second->first, items[static_cast<ptrdiff_t>(i)].second->second));
        target = ::google::protobuf::internal::WireFormatLite::
                   InternalWriteMessageNoVirtualToArray(
                       2, *entry, deterministic, target);
;
      }
    } else {
      ::std::unique_ptr<ProtoStops_IndexesByIdEntry_DoNotUse> entry;
      for (::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 >::const_iterator
          it = this->indexes_by_id().begin();
          it != this->indexes_by_id().end(); ++it) {
        entry.reset(indexes_by_id_.NewEntryWrapper(
            it->first, it->second));
        target = ::google::protobuf::internal::WireFormatLite::
                   InternalWriteMessageNoVirtualToArray(
                       2, *entry, deterministic, target);
;
      }
    }
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:ProtoStops)
  return target;
}

size_t ProtoStops::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:ProtoStops)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // repeated .ProtoStop stops = 1;
  {
    unsigned int count = static_cast<unsigned int>(this->stops_size());
    total_size += 1UL * count;
    for (unsigned int i = 0; i < count; i++) {
      total_size +=
        ::google::protobuf::internal::WireFormatLite::MessageSize(
          this->stops(static_cast<int>(i)));
    }
  }

  // map<int64, int64> indexes_by_id = 2;
  total_size += 1 *
      ::google::protobuf::internal::FromIntSize(this->indexes_by_id_size());
  {
    ::std::unique_ptr<ProtoStops_IndexesByIdEntry_DoNotUse> entry;
    for (::google::protobuf::Map< ::google::protobuf::int64, ::google::protobuf::int64 >::const_iterator
        it = this->indexes_by_id().begin();
        it != this->indexes_by_id().end(); ++it) {
      entry.reset(indexes_by_id_.NewEntryWrapper(it->first, it->second));
      total_size += ::google::protobuf::internal::WireFormatLite::
          MessageSizeNoVirtual(*entry);
    }
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void ProtoStops::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:ProtoStops)
  GOOGLE_DCHECK_NE(&from, this);
  const ProtoStops* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const ProtoStops>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:ProtoStops)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:ProtoStops)
    MergeFrom(*source);
  }
}

void ProtoStops::MergeFrom(const ProtoStops& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:ProtoStops)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  stops_.MergeFrom(from.stops_);
  indexes_by_id_.MergeFrom(from.indexes_by_id_);
}

void ProtoStops::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:ProtoStops)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ProtoStops::CopyFrom(const ProtoStops& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:ProtoStops)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ProtoStops::IsInitialized() const {
  return true;
}

void ProtoStops::Swap(ProtoStops* other) {
  if (other == this) return;
  InternalSwap(other);
}
void ProtoStops::InternalSwap(ProtoStops* other) {
  using std::swap;
  CastToBase(&stops_)->InternalSwap(CastToBase(&other->stops_));
  indexes_by_id_.Swap(&other->indexes_by_id_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
}

::google::protobuf::Metadata ProtoStops::GetMetadata() const {
  protobuf_proto_5fstops_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_proto_5fstops_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
namespace google {
namespace protobuf {
template<> GOOGLE_PROTOBUF_ATTRIBUTE_NOINLINE ::ProtoStops_IndexesByIdEntry_DoNotUse* Arena::CreateMaybeMessage< ::ProtoStops_IndexesByIdEntry_DoNotUse >(Arena* arena) {
  return Arena::CreateInternal< ::ProtoStops_IndexesByIdEntry_DoNotUse >(arena);
}
template<> GOOGLE_PROTOBUF_ATTRIBUTE_NOINLINE ::ProtoStops* Arena::CreateMaybeMessage< ::ProtoStops >(Arena* arena) {
  return Arena::CreateInternal< ::ProtoStops >(arena);
}
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
