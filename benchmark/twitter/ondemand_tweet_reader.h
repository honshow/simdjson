#ifndef TWITTER_ONDEMAND_TWEET_READER_H
#define TWITTER_ONDEMAND_TWEET_READER_H

#include "simdjson.h"
#include "tweet.h"
#include <vector>

SIMDJSON_TARGET_HASWELL

namespace twitter {
namespace {

using namespace simdjson;
using namespace haswell;
using namespace haswell::stage2;

struct ondemand_tweet_reader {
  std::vector<tweet> tweets;
  ondemand::parser parser;

  ondemand_tweet_reader();
  error_code read_tweets(padded_string &json);
  void clear();
}; // struct ondemand_tweet_reader

ondemand_tweet_reader::ondemand_tweet_reader() : tweets{}, parser{} {
}

simdjson_really_inline uint64_t nullable_int(ondemand::value && value) {
  if (value.is_null()) { return 0; }
  return std::move(value);
}

error_code ondemand_tweet_reader::read_tweets(padded_string &buf) {
  tweets.clear();

  // Walk the document, parsing the tweets as we go
  auto doc = parser.parse(buf);
  for (ondemand::object tweet : doc["statuses"]) {
    twitter::tweet t;
    printf("created_at\n");
    t.created_at = tweet["created_at"];
    printf("id\n");
    t.id = tweet["id"];
    printf("text\n");
    t.text = tweet["text"];
    printf("in_reply_to_status_id\n");
    t.in_reply_to_status_id = nullable_int(tweet["in_reply_to_status_id"]);
    {
    printf("user\n");
      ondemand::object user = tweet["user"];
    printf("user.id\n");
      t.user.id = user["id"];
    printf("user.screen_name\n");
      t.user.screen_name = user["screen_name"];
    }
    printf("retweet_count\n");
    t.retweet_count = tweet["retweet_count"];
    printf("favorite_count\n");
    t.favorite_count = tweet["favorite_count"];
    tweets.push_back(t);
  }
  throw TAPE_ERROR;
  // return SUCCESS;
}

} // unnamed namespace
} // namespace twitter

SIMDJSON_UNTARGET_REGION

#endif // TWITTER_SAX_TWEET_READER_H