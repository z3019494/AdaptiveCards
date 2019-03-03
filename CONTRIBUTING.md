This is an adaptation of [DEP engineering planning](https://osgwiki.com/wiki/DEP_Engineering_Planning) procedures to fit
AdaptiveCards engineering.

## Work Units

AdaptiveCards is planned and shipped in releases. Each release is comprised of a scoped collection of features and their
attendant tasks. Features are usually pulled from customer or community requests or are ideas from our PM/dev team. 

Each feature is tracked as an `Epic` in ZenHub. The Epic's description contains requirements, implementation details, and
any supporting documentation, discussion, or other context as needed. Tasks attached to an `Epic` track the implementation
of the feature in each of our supported rendering platforms as appropriate. Every task represents the authoring of tests
and other validation collateral in addition to the actual feature implementation. We also have a single `Epic` to track
updating documentation for all of the other features in the release. Each task has an implementation estimate associated
with it expressed as *Story Points* (equivalent to days).

## Planning

Inputs:

* Possible new features
* Features that missed the cut line in prior releases (backlog)
* Desired ship date (account for stabilization time)

During planning, all features under consideration are given a rough spec, a rough cost estimate, and a priority, which
is determined by customer requirements and how critical a given feature is to a successful release. Total capacity is
computed (`[Number of devs]*[Number of work days in release]+[Additional resources (partner teams)]`). After arranging
features by priority, their costs are compared against the total capacity. Features that fit within 80% of capacity are
considered `Committed`. Features between 80%-100% capacity are considered `At Risk`. A *cut line* is then established at
the 100% capacity line.

At this point, usually a gut check is required. Can we ship a release with only the features above the line? If not,
inputs may need to be changed - can we add more developer time? are our priorities realistic? Reexamine inputs and
iterate until a tenable plan is reached.

Once we pass our gut check, we enter the second phase of planning. Specs are written in greater detail with input from
devs. Devs break work down into smaller tasks (no longer than 5 days). Estimates are reexamined and plan adjustments are
made (e.g. if some work ends up looking more expensive than during the first planning phase, the feature scope may need
adjustment, other features might need to be cut, etc.).

Outputs:

* Release created in ZenHub with start/end dates
* Epics for each feature assigned to release
* Tasks attached to each epic assigned to release
* Cost estimates for each Task

Validation (in ZenHub):

* Verify that all features are assigned to release and have all subtasks and estimates assigned.
* Make sure *Release report* looks correct

## Scheduling

Inputs: Planning

Dependencies are examined to determine execution order. Tasks are placed into milestones and then assigned to devs,
taking care to make sure that devs are evenly loaded.

Outputs:

* Milestones for the expected release length
* Tasks placed into expected milestone and assigned to devs

Validation (in ZenHub):

* Verify that every task is assigned to a milestone and a dev
* Make sure *Burndown report* looks correct for each milestone in the release

## Execution

Inputs: Scheduling

### Team
**Milestone exit/entry:**

* Evaluate estimates for tasks assigned to milestone
* Move tasks as needed for expected amount of work
* Evaluate and close Epics
* Make sure documentation tasks are generated for closed epics
* Review *Velocity tracking* and *Cumulative flow* reports
* Triage bugs
* Consider prereleases (alphas/betas) to gather community feedback if the amount of progress warrants it.

**Issue triage/Project pulse (at least weekly):**

Bugs reported during feature milestones are triaged:

* Bug tagged with `Bug`
* Servicing issues are marked with a servicing label
* Issues that need to be fixed before release are put into the current release
* Issues that need to be fixed before milestone exit are put into the current milestone
* Outstanding bugs are triaged/reviewed and tagged with the new release if needed

### Individual
**Iteration:**

1. Developer chooses task in current milestone to implement (talking to lead and pulling forward from next milestone if
   needed)
2. Dev moves task to `In Progress` in ZenHub
3. When task is ready, dev creates a pull request, moving the task to `Review/QA`
4. Once PR is approved, dev merges the commit and closes the task
5. `goto 1;`

Outputs:

* Closed tasks, bugs, and epics

### Completion
Validation (in ZenHub):

* *Burndown report* is reviewed and discrepancies accounted for/resolved (forgot to burn down days, found work, etc.) 
* *Release report* is reviewed

## Stabilization/Shipping

Inputs:

* All Milestones completed

Once all milestones have been executed, it's time to reexamine our current state. Triage all new bugs. Ensure all
Epics/Tasks/Bugs for the release are completed. Any work left undone has to be reconsidered - Can we ship without it?
How much time do we need to complete it? 

Examine release collateral. We need documentation, blog posts, conversations with our partners and community, conference
materials, etc.

When we're satisfied with what's in the release (or a plan to finalize what's in the release), it's a good time to
consider publishing a *release candidate*. We want some time to gather information about how well the release works,
identify and ameliorate gaps in test coverage and documentation, etc. Iterate on bugs and develop a sense of quality.
This stablization period should occur over a fixed timeframe as considered way back when the release was being planned
initially. Additional stablization periods may need to be considered as appropriate if more certainty about quality is
needed.

Outputs:

* A shiny new release
* Information about execution that can be used to refine planning for the next release

## The Windows <-> ZenHub interface

The [DEP engineering planning guide](https://osgwiki.com/wiki/DEP_Engineering_Planning) describes much of what's in this
document, with a focus on engineering from within the Windows codebase/process. As an expediency for overall Windows org
planning, our releases and milestones are generally planned to align with Windows releases. We use a tool called
[GitHubSync](https://mscodehub.visualstudio.com/_git/GitHubSync) to replicate our GitHub-/ZenHub-tracked Epics and tasks
into the more traditional ADO/VSO/TFS deliverables and workitems that the Windows team is used to. At time of writing,
this replication is one-way (`ADO <- GitHub`) and
[runs](https://microsoft.visualstudio.com/AdaptiveCards/_release?definitionId=10) at the beginning and end of the work
day. In this way, our team's work, capacity, planning, etc. are made visible to standard Windows org reporting tools.
