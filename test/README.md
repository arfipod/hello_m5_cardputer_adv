# Tests

The `native` environment runs host-side tests for portable code in `lib/common`.

Run:

```sh
pio test -e native
```

On Windows, `platform = native` requires a host C/C++ compiler on PATH.
