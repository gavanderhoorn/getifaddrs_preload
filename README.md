
## Deprecated

Note: as of Win10 build 14965, the prelod trick is NOT necessary anymore.

See also [issue468][] on the BashOnWindows issue tracker.

If you are still having trouble ('permission denied' errors in Python
`netifaces.interfaces()` for instance), see [teredo_trouble][].


## Usage

```shell
$ LD_PRELOAD=/path/to/_getifaddrs.so /path/to/binary
```


[issue468]: https://github.com/Microsoft/BashOnWindows/issues/468
[teredo_trouble]: https://github.com/Microsoft/BashOnWindows/issues/468#issuecomment-259662271
