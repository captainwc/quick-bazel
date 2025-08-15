refresh:
	bazel run @hedron_compile_commands//:refresh_all

clean:
	bazel clean --expunge

build-all:
	bazel build //...
