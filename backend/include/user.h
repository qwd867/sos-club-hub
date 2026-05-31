#ifndef USER_H
#define USER_H

#include <cstdint>
#include <string>

class User {
public:
    User();
    User(int64_t id, const std::string& email, const std::string& password_hash,
         const std::string& username, int64_t created_at, int64_t updated_at);

    int64_t get_id() const;
    const std::string& get_email() const;
    const std::string& get_password_hash() const;
    const std::string& get_username() const;
    int64_t get_created_at() const;
    int64_t get_updated_at() const;
    int64_t get_quest_points() const;

    void set_id(int64_t id);
    void set_email(const std::string& email);
    void set_password_hash(const std::string& password_hash);
    void set_username(const std::string& username);
    void set_created_at(int64_t created_at);
    void set_updated_at(int64_t updated_at);
    void set_quest_points(int64_t quest_points);

private:
    int64_t id_;
    std::string email_;
    std::string password_hash_;
    std::string username_;
    int64_t created_at_;
    int64_t updated_at_;
    int64_t quest_points_;
};

#endif
