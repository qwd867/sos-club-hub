#include "user.h"

User::User() : id_(0), created_at_(0), updated_at_(0), quest_points_(0) {}

User::User(int64_t id, const std::string& email, const std::string& password_hash,
           const std::string& username, int64_t created_at, int64_t updated_at)
    : id_(id), email_(email), password_hash_(password_hash),
      username_(username), created_at_(created_at), updated_at_(updated_at), quest_points_(0) {}

int64_t User::get_id() const { return id_; }
const std::string& User::get_email() const { return email_; }
const std::string& User::get_password_hash() const { return password_hash_; }
const std::string& User::get_username() const { return username_; }
int64_t User::get_created_at() const { return created_at_; }
int64_t User::get_updated_at() const { return updated_at_; }

void User::set_id(int64_t id) { id_ = id; }
void User::set_email(const std::string& email) { email_ = email; }
void User::set_password_hash(const std::string& password_hash) { password_hash_ = password_hash; }
void User::set_username(const std::string& username) { username_ = username; }
void User::set_created_at(int64_t created_at) { created_at_ = created_at; }
void User::set_updated_at(int64_t updated_at) { updated_at_ = updated_at; }
int64_t User::get_quest_points() const { return quest_points_; }
void User::set_quest_points(int64_t quest_points) { quest_points_ = quest_points; }
