// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __STOUT_OS_POSIX_STAT_HPP__
#define __STOUT_OS_POSIX_STAT_HPP__

#include <sys/stat.h>
#include <sys/statvfs.h>

#include <string>

#include <stout/bytes.hpp>
#include <stout/try.hpp>
#include <stout/unreachable.hpp>


namespace os {

namespace stat {

namespace internal {

inline Try<struct ::stat> stat(
    const std::string& path,
    const FollowSymlink follow)
{
  struct ::stat s;

  switch (follow) {
    case DO_NOT_FOLLOW_SYMLINK:
      if (::lstat(path.c_str(), &s) < 0) {
        return ErrnoError("Failed to lstat '" + path + "'");
      }
      return s;
    case FOLLOW_SYMLINK:
      if (::stat(path.c_str(), &s) < 0) {
        return ErrnoError("Failed to stat '" + path + "'");
      }
      return s;
  }

  UNREACHABLE();
}

} // namespace internal {

inline bool islink(const std::string& path)
{
  // By definition, you don't followsymlinks when trying
  // to find whether a path is a link. If you followed it,
  // it wouldn't ever be a link.
  Try<struct ::stat> s = internal::stat(path, DO_NOT_FOLLOW_SYMLINK);
  return s.isSome() && S_ISLNK(s->st_mode);
}


inline bool isdir(
    const std::string& path,
    const FollowSymlink follow = FOLLOW_SYMLINK)
{
  Try<struct ::stat> s = internal::stat(path, follow);
  return s.isSome() && S_ISDIR(s->st_mode);
}


inline bool isfile(
    const std::string& path,
    const FollowSymlink follow = FOLLOW_SYMLINK)
{
  Try<struct ::stat> s = internal::stat(path, follow);
  return s.isSome() && S_ISREG(s->st_mode);
}


// Returns the size in Bytes of a given file system entry. When
// applied to a symbolic link with `follow` set to
// `DO_NOT_FOLLOW_SYMLINK`, this will return the length of the entry
// name (strlen).
inline Try<Bytes> size(
    const std::string& path,
    const FollowSymlink follow = FOLLOW_SYMLINK)
{
  Try<struct ::stat> s = internal::stat(path, follow);
  if (s.isError()) {
    return Error(s.error());
  }

  return Bytes(s->st_size);
}


inline Try<long> mtime(
    const std::string& path,
    const FollowSymlink follow = FOLLOW_SYMLINK)
{
  Try<struct ::stat> s = internal::stat(path, follow);
  if (s.isError()) {
    return Error(s.error());
  }

  return s->st_mtime;
}


inline Try<mode_t> mode(
    const std::string& path,
    const FollowSymlink follow = FOLLOW_SYMLINK)
{
  Try<struct ::stat> s = internal::stat(path, follow);
  if (s.isError()) {
    return Error(s.error());
  }

  return s->st_mode;
}


inline Try<dev_t> dev(
    const std::string& path,
    const FollowSymlink follow = FOLLOW_SYMLINK)
{
  Try<struct ::stat> s = internal::stat(path, follow);
  if (s.isError()) {
    return Error(s.error());
  }

  return s->st_dev;
}


inline Try<dev_t> rdev(
    const std::string& path,
    const FollowSymlink follow = FOLLOW_SYMLINK)
{
  Try<struct ::stat> s = internal::stat(path, follow);
  if (s.isError()) {
    return Error(s.error());
  }

  if (!S_ISCHR(s->st_mode) && !S_ISBLK(s->st_mode)) {
    return Error("Not a special file: " + path);
  }

  return s->st_rdev;
}


inline Try<ino_t> inode(
    const std::string& path,
    const FollowSymlink follow = FOLLOW_SYMLINK)
{
  Try<struct ::stat> s = internal::stat(path, follow);
  if (s.isError()) {
    return Error(s.error());
  }

  return s->st_ino;
}


inline Try<uid_t> uid(
    const std::string& path,
    const FollowSymlink follow = FOLLOW_SYMLINK)
{
  Try<struct ::stat> s = internal::stat(path, follow);
  if (s.isError()) {
    return Error(s.error());
  }

  return s->st_uid;
}

} // namespace stat {

} // namespace os {

#endif // __STOUT_OS_POSIX_STAT_HPP__
