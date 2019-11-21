# ReleaseNotes

## Current version

| Date       | Version     |
|------------|-------------|
| 19.11.2019 | 1.2.0       |

### New Features

- "Publish On Change".

  There is now the option to publish values only when they change, rather than on a fixed time period.
  The `publish_on_change` switch applies to all published variables, i.e. all variables must be published either on change, or on fixed period(s).
  The maximum publish frequency remains at 2 Hz (i.e. 500 ms period). To assist with this feature, users can configure a GDS port to receive an integer that increments after each publish cycle. This `cycle_count_port` value can be used as a clock to feed messages into GDS variables from a buffer, if necessary.

### Changes

### BugFixes

- The `username` configuration variable now works as intended.

----------

## Previous versions

| Date       | Version     |
|------------|-------------|
| 25.05.2019 | 1.1.1       |

### New Features

- None.

### Changes

### BugFixes

- App no longer crashes after reconnecting to broker.
- Publishing now occurs at the correct frequency.

----------

| Date       | Version     |
|------------|-------------|
| 02.05.2019 | 1.1.0       |

### New Features

- A publish period can now be specified for each publish item.
- Connection status is now written to a user-configurable GDS port.
- A user-configurable GDS port variable can be used to trigger a server reconnect attempt.
- When configured, automatic reconnect attempts will continue indefinitely until connection is re-established.

### Changes

### BugFixes

----------

| Date       | Version     |
|------------|-------------|
| 29.03.2019 | 1.0.2       |

### New Features

### Changes

### BugFixes
