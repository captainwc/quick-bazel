def _idl_compile_impl(ctx):
    idl_basename = ctx.file.src.basename.replace(".idl", "")
    input_dir = ctx.file.src.dirname
    outputs = [
        ctx.actions.declare_file(idl_basename + ext)
        for ext in [
            ".hpp",
            "PubSubTypes.hpp",
            "PubSubTypes.cxx",
            "TypeObjectSupport.hpp",
            "TypeObjectSupport.cxx",
            "CdrAux.hpp",
            "CdrAux.ipp",
        ]
    ]
    command = """
    {gen_tool} -replace -cs {idl_file}
    mv "./{input_dir}/{basename}.hpp" {hpp_output}
    mv "./{input_dir}/{basename}PubSubTypes.hpp" {pubsub_hpp_output}
    mv "./{input_dir}/{basename}PubSubTypes.cxx" {pubsub_cxx_output}
    mv "./{input_dir}/{basename}TypeObjectSupport.hpp" {typeobj_hpp_output}
    mv "./{input_dir}/{basename}TypeObjectSupport.cxx" {typeobj_cxx_output}
    mv "./{input_dir}/{basename}CdrAux.hpp" {cdraux_hpp_output}
    mv "./{input_dir}/{basename}CdrAux.ipp" {cdraux_ipp_output}
    """.format(
        gen_tool = ctx.executable.fastddsgen.path,
        idl_file = ctx.file.src.path,
        input_dir = input_dir,
        basename = idl_basename,
        hpp_output = outputs[0].path,
        pubsub_hpp_output = outputs[1].path,
        pubsub_cxx_output = outputs[2].path,
        typeobj_hpp_output = outputs[3].path,
        typeobj_cxx_output = outputs[4].path,
        cdraux_hpp_output = outputs[5].path,
        cdraux_ipp_output = outputs[6].path,
    )
    ctx.actions.run_shell(
        tools = [ctx.executable.fastddsgen],
        inputs = [ctx.file.src, ctx.executable.fastddsgen] + ctx.files.jarfile,
        outputs = outputs,
        command = command,
        env = {"PATH": "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"},
    )
    return [DefaultInfo(files = depset(outputs))]

_idl_compile = rule(
    implementation = _idl_compile_impl,
    attrs = {
        "src": attr.label(allow_single_file = True, mandatory = True),
        "fastddsgen": attr.label(
            executable = True,
            cfg = "exec",
            allow_single_file = True,
            default = Label("//tools:fastddsgen_bin"),
        ),
        "jarfile": attr.label_list(
            allow_files = True,
            default = [Label("//tools:fastddsgen_jar")],
        ),
    },
)

def idl_library(name, src, deps = [], fastdds_deps = ["@fastdds//:fastdds"]):
    gen_target = name + "_gen"
    _idl_compile(name = gen_target, src = src)
    native.cc_library(
        name = name,
        srcs = [":" + gen_target],
        hdrs = [":" + gen_target],
        includes = ["."],
        deps = deps + fastdds_deps,
    )
