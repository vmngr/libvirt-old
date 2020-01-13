{
    "targets": [
        {
            "target_name": "virt",
            "product_prefix": "lib",
            "cflags!": [ "-fno-exceptions" ],
            "cflags_cc!": [ "-fno-exceptions" ],
            "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
            "sources": [
                "src/libvirt.cc",
                "src/worker.cc",
                "src/hypervisor.cc",
                "src/hypervisor-connect.cc",
                "src/hypervisor-domain.cc",
                "src/hypervisor-node.cc",
                "src/domain.cc"
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                ".",
                "/usr/include/glib-2.0",
                "/usr/lib/glib-2.0/include",
                "/usr/lib/x86_64-linux-gnu/glib-2.0/include"
            ],
            "link_settings": {
                "libraries": [ "-lvirt" , "-lglib-2.0" ]
            }
        }
    ]
}
