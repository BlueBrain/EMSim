# Contributing to EMSim

We would love for you to contribute to EMSim and help make it even better than it is today! As a
contributor, here are the guidelines we would like you to follow:
 - [Question or Problem?](#got-a-question-or-problem)
 - [Issues and Bugs](#found-a-bug)
 - [Feature Requests](#missing-a-feature)
 - [Submission Guidelines](#submission-guidelines)

## Got a Question or Problem?

Please do not hesitate to raise an issue on [github project page][github].

## Found a Bug?

If you find a bug in the source code, you can help us by [submitting an issue](#submitting-an-issue) to our
[GitHub Repository][github]. Even better, you can [submit a Pull Request](#submitting-a-pull-request) with a fix.

## Missing a Feature?

You can *request* a new feature by [submitting an issue](#submitting-an-issue) to our GitHub Repository. If you would like to
*implement* a new feature, please submit an issue with a proposal for your work first, to be sure that we can use it.

Please consider what kind of change it is:
* For a **Major Feature**, first open an issue and outline your proposal so that it can be
discussed. This will also allow us to better coordinate our efforts, prevent duplication of work,
and help you to craft the change so that it is successfully accepted into the project.
* **Small Features** can be crafted and directly [submitted as a Pull Request](#submitting-a-pull-request).

## Submission Guidelines

### Submitting an Issue

Before you submit an issue, please search the issue tracker, maybe an issue for your problem already exists and the
discussion might inform you of workarounds readily available.

We want to fix all the issues as soon as possible, but before fixing a bug we need to reproduce and confirm it. In order
to reproduce bugs we will need as much information as possible, and preferably be in touch with you to gather
information.

### Submitting a Pull Request

When you wish to contribute to the code base, please consider the following guidelines:
* Make a [fork](https://guides.github.com/activities/forking/) of this repository.
* Make your changes in your fork, in a new git branch:
     ```shell
     git checkout -b my-fix-branch master
     ```
* Create your patch, **including appropriate test cases**.
* Run the full test suite, and ensure that all tests pass.
* Commit your changes using a descriptive commit message.
     ```shell
     git commit -a
     ```
  Note: the optional commit `-a` command line option will automatically “add” and “rm” edited files.
* Push your branch to GitHub:
    ```shell
    git push origin my-fix-branch
    ```
* Please sign our [Contributor License Agreement (CLA)](#cla) before sending Pull Requests (PR).
  We cannot accept any contribution without your signature.
* In GitHub, send a Pull Request to the `master` branch of the upstream repository of the relevant component.
* If we suggest changes then:
  * Make the required updates.
  * Re-run the test suites to ensure tests are still passing.
  * Rebase your branch and force push to your GitHub repository (this will update your Pull Request):
    ```shell
    git rebase master -i
    git push -f
    ```
That’s it! Thank you for your contribution!

#### After your pull request is merged

After your pull request is merged, you can safely delete your branch and pull the changes from the main (upstream)
repository:
* Delete the remote branch on GitHub either through the GitHub web UI or your local shell as follows:
    ```shell
    git push origin --delete my-fix-branch
    ```
* Check out the master branch:
    ```shell
    git checkout master -f
    ```
* Delete the local branch:
    ```shell
    git branch -D my-fix-branch
    ```
* Update your master with the latest upstream version:
    ```shell
    git pull --ff upstream master
    ```
[github]: https://github.com/BlueBrain/EMSim