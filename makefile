#	bazel run @hedron_compile_commands//:refresh_all

refresh:
	bazel-compile-commands //...

clean:
	bazel clean --expunge

build-all:
	bazel build //...
