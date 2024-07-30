/*  cbvcs Code::Blocks version control system plugin

    Copyright (C) 2011 Dushara Jayasinghe.
    Copyright (C) 2024 Christo Joseph

    cbvcs is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cbvcs is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cbvcs.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "git_libgit2.h"
#include "icommandexecuter.h"
#include <git2.h>
#include <wx/string.h>

LibGit2::LibGit2(const wxString &project, ICommandExecuter &cmdExecutor)
    : IVersionControlSystem(project, m_GitUpdate, m_GitAdd, m_GitRemove, m_GitCommit, m_GitRevert),
      m_CmdExecutor(cmdExecutor),
      m_GitUpdate(*this, m_GitRoot, m_CmdExecutor),
      m_GitAdd(*this, m_GitRoot, m_CmdExecutor),
      m_GitRemove(*this, m_GitRoot, m_CmdExecutor),
      m_GitCommit(*this, m_GitRoot, m_CmdExecutor),
      m_GitRevert(*this, m_GitRoot, m_CmdExecutor)
{
    git_libgit2_init();
    m_GitRoot = QueryRoot(project);
}

LibGit2::~LibGit2() { git_libgit2_shutdown(); }

wxString LibGit2::QueryRoot(const wxString &project)
{
    git_buf root = {0};
    wxString ret;
    git_libgit2_init();
    int error = git_repository_discover(&root, wxPathOnly(project).ToUTF8().data(), 0, NULL);
    if (!error)
    {
        git_repository *repo;
        error = git_repository_open(&repo, wxPathOnly(project).ToUTF8().data());
        if (0 == error)
        {
            const char *workDir = git_repository_workdir(repo);
            ret = wxString(workDir);
            git_repository_free(repo);
        }
        else
        {
            const git_error *e = git_error_last();
            fprintf(stderr, "LibGit2::%s:%d git_repository_open failed : %d/%d: %s\n", __FUNCTION__, __LINE__, error, e->klass, e->message);
        }
    }
    else
    {
        const git_error *e = git_error_last();
        fprintf(stderr, "LibGit2::%s:%d git_repository_discover failed : %d/%d: %s\n", __FUNCTION__, __LINE__, error, e->klass, e->message);
    }
    git_buf_free(&root);
    git_libgit2_shutdown();
    fprintf(stderr, "LibGit2::%s:%d return root : %s\n", __FUNCTION__, __LINE__, ret.ToUTF8().data());

    return ret;
}

/*static*/ bool LibGit2::IsGitRepo(const wxString &project, ICommandExecuter &shellUtils)
{
    bool ret;
    git_repository *repo;
    git_libgit2_init();
    if (0 == git_repository_open(&repo, wxPathOnly(project).ToUTF8().data()))
    {
        fprintf(stderr, "LibGit2::%s:%d yes repo\n", __FUNCTION__, __LINE__);
        ret = true;
        git_repository_free(repo);
    }
    else
    {
        fprintf(stderr, "LibGit2::%s:%d not repo\n", __FUNCTION__, __LINE__);
        ret = false;
    }
    git_libgit2_shutdown();
    return ret;
}

#if 0

void GitRevertOp::execute(std::vector<VcsTreeItem*>& paths) const
{
}
#endif
