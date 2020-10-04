// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_RSSDATA_H_
#define FLATBUFFERS_GENERATED_RSSDATA_H_

#include "flatbuffers/flatbuffers.h"

namespace mfeed_fb {
namespace rss_data {

struct Item;
struct ItemBuilder;

struct Feed;
struct FeedBuilder;

struct RSSData;
struct RSSDataBuilder;

struct Item FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ItemBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TITLE = 4,
    VT_LINK = 6,
    VT_GUID = 8,
    VT_DESCRIPTION = 10,
    VT_PUB_DATE = 12
  };
  const flatbuffers::String *title() const {
    return GetPointer<const flatbuffers::String *>(VT_TITLE);
  }
  const flatbuffers::String *link() const {
    return GetPointer<const flatbuffers::String *>(VT_LINK);
  }
  const flatbuffers::String *guid() const {
    return GetPointer<const flatbuffers::String *>(VT_GUID);
  }
  const flatbuffers::String *description() const {
    return GetPointer<const flatbuffers::String *>(VT_DESCRIPTION);
  }
  const flatbuffers::String *pub_date() const {
    return GetPointer<const flatbuffers::String *>(VT_PUB_DATE);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_TITLE) &&
           verifier.VerifyString(title()) &&
           VerifyOffset(verifier, VT_LINK) &&
           verifier.VerifyString(link()) &&
           VerifyOffset(verifier, VT_GUID) &&
           verifier.VerifyString(guid()) &&
           VerifyOffset(verifier, VT_DESCRIPTION) &&
           verifier.VerifyString(description()) &&
           VerifyOffset(verifier, VT_PUB_DATE) &&
           verifier.VerifyString(pub_date()) &&
           verifier.EndTable();
  }
};

struct ItemBuilder {
  typedef Item Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_title(flatbuffers::Offset<flatbuffers::String> title) {
    fbb_.AddOffset(Item::VT_TITLE, title);
  }
  void add_link(flatbuffers::Offset<flatbuffers::String> link) {
    fbb_.AddOffset(Item::VT_LINK, link);
  }
  void add_guid(flatbuffers::Offset<flatbuffers::String> guid) {
    fbb_.AddOffset(Item::VT_GUID, guid);
  }
  void add_description(flatbuffers::Offset<flatbuffers::String> description) {
    fbb_.AddOffset(Item::VT_DESCRIPTION, description);
  }
  void add_pub_date(flatbuffers::Offset<flatbuffers::String> pub_date) {
    fbb_.AddOffset(Item::VT_PUB_DATE, pub_date);
  }
  explicit ItemBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ItemBuilder &operator=(const ItemBuilder &);
  flatbuffers::Offset<Item> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Item>(end);
    return o;
  }
};

inline flatbuffers::Offset<Item> CreateItem(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> title = 0,
    flatbuffers::Offset<flatbuffers::String> link = 0,
    flatbuffers::Offset<flatbuffers::String> guid = 0,
    flatbuffers::Offset<flatbuffers::String> description = 0,
    flatbuffers::Offset<flatbuffers::String> pub_date = 0) {
  ItemBuilder builder_(_fbb);
  builder_.add_pub_date(pub_date);
  builder_.add_description(description);
  builder_.add_guid(guid);
  builder_.add_link(link);
  builder_.add_title(title);
  return builder_.Finish();
}

inline flatbuffers::Offset<Item> CreateItemDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *title = nullptr,
    const char *link = nullptr,
    const char *guid = nullptr,
    const char *description = nullptr,
    const char *pub_date = nullptr) {
  auto title__ = title ? _fbb.CreateString(title) : 0;
  auto link__ = link ? _fbb.CreateString(link) : 0;
  auto guid__ = guid ? _fbb.CreateString(guid) : 0;
  auto description__ = description ? _fbb.CreateString(description) : 0;
  auto pub_date__ = pub_date ? _fbb.CreateString(pub_date) : 0;
  return CreateItem(
      _fbb,
      title__,
      link__,
      guid__,
      description__,
      pub_date__);
}

struct Feed FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef FeedBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_URL = 4,
    VT_TMP_PATH = 6,
    VT_TITLE = 8,
    VT_LINK = 10,
    VT_DESCRIPTION = 12,
    VT_LANGUAGE = 14,
    VT_ITEMS = 16,
    VT_OPEN_WITH = 18,
    VT_TAGS = 20
  };
  const flatbuffers::String *url() const {
    return GetPointer<const flatbuffers::String *>(VT_URL);
  }
  const flatbuffers::String *tmp_path() const {
    return GetPointer<const flatbuffers::String *>(VT_TMP_PATH);
  }
  const flatbuffers::String *title() const {
    return GetPointer<const flatbuffers::String *>(VT_TITLE);
  }
  const flatbuffers::String *link() const {
    return GetPointer<const flatbuffers::String *>(VT_LINK);
  }
  const flatbuffers::String *description() const {
    return GetPointer<const flatbuffers::String *>(VT_DESCRIPTION);
  }
  const flatbuffers::String *language() const {
    return GetPointer<const flatbuffers::String *>(VT_LANGUAGE);
  }
  const flatbuffers::Vector<flatbuffers::Offset<Item>> *items() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Item>> *>(VT_ITEMS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *open_with() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_OPEN_WITH);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *tags() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_TAGS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_URL) &&
           verifier.VerifyString(url()) &&
           VerifyOffset(verifier, VT_TMP_PATH) &&
           verifier.VerifyString(tmp_path()) &&
           VerifyOffset(verifier, VT_TITLE) &&
           verifier.VerifyString(title()) &&
           VerifyOffset(verifier, VT_LINK) &&
           verifier.VerifyString(link()) &&
           VerifyOffset(verifier, VT_DESCRIPTION) &&
           verifier.VerifyString(description()) &&
           VerifyOffset(verifier, VT_LANGUAGE) &&
           verifier.VerifyString(language()) &&
           VerifyOffset(verifier, VT_ITEMS) &&
           verifier.VerifyVector(items()) &&
           verifier.VerifyVectorOfTables(items()) &&
           VerifyOffset(verifier, VT_OPEN_WITH) &&
           verifier.VerifyVector(open_with()) &&
           verifier.VerifyVectorOfStrings(open_with()) &&
           VerifyOffset(verifier, VT_TAGS) &&
           verifier.VerifyVector(tags()) &&
           verifier.VerifyVectorOfStrings(tags()) &&
           verifier.EndTable();
  }
};

struct FeedBuilder {
  typedef Feed Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_url(flatbuffers::Offset<flatbuffers::String> url) {
    fbb_.AddOffset(Feed::VT_URL, url);
  }
  void add_tmp_path(flatbuffers::Offset<flatbuffers::String> tmp_path) {
    fbb_.AddOffset(Feed::VT_TMP_PATH, tmp_path);
  }
  void add_title(flatbuffers::Offset<flatbuffers::String> title) {
    fbb_.AddOffset(Feed::VT_TITLE, title);
  }
  void add_link(flatbuffers::Offset<flatbuffers::String> link) {
    fbb_.AddOffset(Feed::VT_LINK, link);
  }
  void add_description(flatbuffers::Offset<flatbuffers::String> description) {
    fbb_.AddOffset(Feed::VT_DESCRIPTION, description);
  }
  void add_language(flatbuffers::Offset<flatbuffers::String> language) {
    fbb_.AddOffset(Feed::VT_LANGUAGE, language);
  }
  void add_items(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Item>>> items) {
    fbb_.AddOffset(Feed::VT_ITEMS, items);
  }
  void add_open_with(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> open_with) {
    fbb_.AddOffset(Feed::VT_OPEN_WITH, open_with);
  }
  void add_tags(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> tags) {
    fbb_.AddOffset(Feed::VT_TAGS, tags);
  }
  explicit FeedBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  FeedBuilder &operator=(const FeedBuilder &);
  flatbuffers::Offset<Feed> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Feed>(end);
    return o;
  }
};

inline flatbuffers::Offset<Feed> CreateFeed(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> url = 0,
    flatbuffers::Offset<flatbuffers::String> tmp_path = 0,
    flatbuffers::Offset<flatbuffers::String> title = 0,
    flatbuffers::Offset<flatbuffers::String> link = 0,
    flatbuffers::Offset<flatbuffers::String> description = 0,
    flatbuffers::Offset<flatbuffers::String> language = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Item>>> items = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> open_with = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> tags = 0) {
  FeedBuilder builder_(_fbb);
  builder_.add_tags(tags);
  builder_.add_open_with(open_with);
  builder_.add_items(items);
  builder_.add_language(language);
  builder_.add_description(description);
  builder_.add_link(link);
  builder_.add_title(title);
  builder_.add_tmp_path(tmp_path);
  builder_.add_url(url);
  return builder_.Finish();
}

inline flatbuffers::Offset<Feed> CreateFeedDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *url = nullptr,
    const char *tmp_path = nullptr,
    const char *title = nullptr,
    const char *link = nullptr,
    const char *description = nullptr,
    const char *language = nullptr,
    const std::vector<flatbuffers::Offset<Item>> *items = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *open_with = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *tags = nullptr) {
  auto url__ = url ? _fbb.CreateString(url) : 0;
  auto tmp_path__ = tmp_path ? _fbb.CreateString(tmp_path) : 0;
  auto title__ = title ? _fbb.CreateString(title) : 0;
  auto link__ = link ? _fbb.CreateString(link) : 0;
  auto description__ = description ? _fbb.CreateString(description) : 0;
  auto language__ = language ? _fbb.CreateString(language) : 0;
  auto items__ = items ? _fbb.CreateVector<flatbuffers::Offset<Item>>(*items) : 0;
  auto open_with__ = open_with ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*open_with) : 0;
  auto tags__ = tags ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*tags) : 0;
  return CreateFeed(
      _fbb,
      url__,
      tmp_path__,
      title__,
      link__,
      description__,
      language__,
      items__,
      open_with__,
      tags__);
}

struct RSSData FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef RSSDataBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_FEEDS_LIST = 4,
    VT_BROWSER = 6
  };
  const flatbuffers::Vector<flatbuffers::Offset<Feed>> *feeds_list() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Feed>> *>(VT_FEEDS_LIST);
  }
  const flatbuffers::String *browser() const {
    return GetPointer<const flatbuffers::String *>(VT_BROWSER);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_FEEDS_LIST) &&
           verifier.VerifyVector(feeds_list()) &&
           verifier.VerifyVectorOfTables(feeds_list()) &&
           VerifyOffset(verifier, VT_BROWSER) &&
           verifier.VerifyString(browser()) &&
           verifier.EndTable();
  }
};

struct RSSDataBuilder {
  typedef RSSData Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_feeds_list(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Feed>>> feeds_list) {
    fbb_.AddOffset(RSSData::VT_FEEDS_LIST, feeds_list);
  }
  void add_browser(flatbuffers::Offset<flatbuffers::String> browser) {
    fbb_.AddOffset(RSSData::VT_BROWSER, browser);
  }
  explicit RSSDataBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  RSSDataBuilder &operator=(const RSSDataBuilder &);
  flatbuffers::Offset<RSSData> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<RSSData>(end);
    return o;
  }
};

inline flatbuffers::Offset<RSSData> CreateRSSData(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Feed>>> feeds_list = 0,
    flatbuffers::Offset<flatbuffers::String> browser = 0) {
  RSSDataBuilder builder_(_fbb);
  builder_.add_browser(browser);
  builder_.add_feeds_list(feeds_list);
  return builder_.Finish();
}

inline flatbuffers::Offset<RSSData> CreateRSSDataDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<Feed>> *feeds_list = nullptr,
    const char *browser = nullptr) {
  auto feeds_list__ = feeds_list ? _fbb.CreateVector<flatbuffers::Offset<Feed>>(*feeds_list) : 0;
  auto browser__ = browser ? _fbb.CreateString(browser) : 0;
  return CreateRSSData(
      _fbb,
      feeds_list__,
      browser__);
}

} // rss_data
} // mfeed_fb

#endif  // FLATBUFFERS_GENERATED_RSSDATA_H_