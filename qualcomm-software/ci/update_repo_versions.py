#!/usr/bin/env python3

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import argparse
import json
import logging
import subprocess
from pathlib import Path

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)

REMOTE_NAME = "origin"


def run_cmd(cmd: list[str], cwd: Path | None = None) -> str:
    result = subprocess.run(
        cmd,
        check=True,
        capture_output=True,
        text=True,
        cwd=cwd,
    )
    return result.stdout.strip()


def update_repo_version(
    repo_path: Path,
    repo_name: str,
    to_branch: str,
    dry_run: bool,
) -> None:
    versions_file = repo_path / "qualcomm-software" / "versions.json"

    logger.info("Switching to branch %s", to_branch)
    run_cmd(["git", "-C", str(repo_path), "switch", to_branch])

    logger.info("Loading %s", versions_file)

    with open(versions_file, encoding="utf-8") as f:
        data = json.load(f)

    repos = data.get("repos", {})

    if repo_name not in repos:
        raise RuntimeError(f"Repository '{repo_name}' not found in versions.json")

    repo_info = repos[repo_name]

    tag_type = repo_info.get("tagType")
    repo_url = repo_info.get("url")
    current_tag = repo_info.get("tag")

    logger.info("Repo: %s", repo_name)
    logger.info("URL: %s", repo_url)
    logger.info("tagType: %s", tag_type)
    logger.info("Current tag: %s", current_tag)

    # Only update repositories already using commit hashes
    if tag_type != "commithash":
        logger.info(
            "Skipping %s because tagType is '%s'",
            repo_name,
            tag_type,
        )
        return

    tracking_branch = repo_info.get("trackingBranch")

    if not tracking_branch:
        raise RuntimeError(
                f"Repository '{repo_name}' is missing 'trackingBranch'"
                )
    branch_ref = f"refs/heads/{tracking_branch}"
    logger.info("Tracking ref: %s", branch_ref)

    latest_sha = run_cmd(
        ["git", "ls-remote", repo_url, branch_ref]
    ).split()[0]

    logger.info("Latest SHA: %s", latest_sha)

    if latest_sha == current_tag:
        logger.info("%s already up to date", repo_name)
        return
    # Update only tag
    repo_info["tag"] = latest_sha

    with open(versions_file, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2)
        f.write("\n")

    logger.info("Updated versions.json")

    # Stage file
    run_cmd(
        ["git", "-C", str(repo_path), "add", str(versions_file)]
    )

    diff_rc = subprocess.run(
        [
            "git",
            "-C",
            str(repo_path),
            "diff",
            "--cached",
            "--quiet",
        ]
    )

    if diff_rc.returncode == 0:
        logger.info("No staged changes")
        return

    commit_msg = f"Automerge: Update {repo_name} to {latest_sha}"

    run_cmd(
        [
            "git",
            "-C",
            str(repo_path),
            "commit",
            "-m",
            commit_msg,
        ]
    )

    logger.info("Created commit: %s", commit_msg)

    if dry_run:
        logger.info("Dry run; skipping push")
        return

    run_cmd(
        [
            "git",
            "-C",
            str(repo_path),
            "push",
            REMOTE_NAME,
            f"HEAD:{to_branch}",
        ]
    )
    logger.info("Push complete")


def main() -> None:
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--repo-name",
        required=True,
        help="Repository name from versions.json (ex: eld)",
    )

    parser.add_argument(
        "--repo-path",
        default=Path.cwd(),
        type=Path,
    )

    parser.add_argument(
        "--to-branch",
        required=True,
        help="Target cpullvm branch",
    )

    parser.add_argument(
        "--dry-run",
        action="store_true",
    )

    args = parser.parse_args()

    try:
        update_repo_version(
            repo_path=args.repo_path,
            repo_name=args.repo_name,
            to_branch=args.to_branch,
            dry_run=args.dry_run,
        )
    except subprocess.CalledProcessError as error:
        logger.error(
            'Failed to run command: "%s"\nstdout:\n%s\nstderr:\n%s',
            " ".join(error.cmd),
            error.stdout,
            error.stderr,
        )
        raise SystemExit(1)

    except Exception as error:
        logger.exception(
            "Failed to update repository '%s': %s",
            args.repo_name,
            str(error),
        )
        raise SystemExit(1)


if __name__ == "__main__":
    main()
