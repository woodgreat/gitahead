//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the MIT License. The LICENSE.md file
// describes the conditions under which this software may be distributed.
//
// Author: Jason Haslam
//

#ifndef DIFF_H
#define DIFF_H

#include "Id.h"
#include "Index.h"
#include "git2/diff.h"
#include <QFlags>
#include <QSharedPointer>

namespace git {

class Patch;

class Diff
{
public:
  enum File
  {
    NewFile,
    OldFile
  };

  enum SortRole
  {
    NameRole,
    StatusRole
  };

  class Callbacks {
  public:
    virtual bool progress(
      const QString &oldPath,
      const QString &newPath)
    {
      return false;
    }

    static int progress(
      const git_diff *diff,
      const char *oldPath,
      const char *newPath,
      void *payload);
  };

  Diff();

  bool isValid() const { return !d.isNull(); }
  explicit operator bool() const { return isValid(); }

  bool isConflicted() const;
  bool isStatusDiff() const;
  Index index() const { return d->index; }

  int count() const;
  Patch patch(int index) const;
  QString name(int index) const;
  bool isBinary(int index) const;
  git_delta_t status(int index) const;
  Id id(int index, File file) const;

  int indexOf(const QString &name) const;

  // Merge the given diff into this diff.
  void merge(const Diff &diff);

  // Detect renames, copies, etc. This is expensive.
  void findSimilar(bool untracked = false);

  void sort(SortRole role, Qt::SortOrder order = Qt::AscendingOrder);

  void setAllStaged(bool staged, bool yieldFocus = true);

  QByteArray toBuffer(git_diff_format_t format = GIT_DIFF_FORMAT_PATCH) const;

  static char statusChar(git_delta_t status);

  static Diff fromBuffer(const QByteArray &text);

private:
  struct Data
  {
    Data(git_diff *diff);
    ~Data();

    void resetMap();
    const git_diff_delta *delta(int index) const;

    git_diff *diff;
    QList<int> map;
    Index index;
  };

  Diff(git_diff *diff);
  operator git_diff *() const;
  void setIndex(const Index &index);

  QSharedPointer<Data> d;

  friend class Commit;
  friend class Repository;
};

} // namespace git

Q_DECLARE_METATYPE(git::Diff);

#endif
