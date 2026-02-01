SECTIONS
{
    .objc_metadata : 
    {
        . = ALIGN(8);
        __objc_runtime_start = .;
        
        KEEP(*(.objc_info))
        __objc_module_list_start = .;
        KEEP(*(.objc_module_info)) 
        __objc_module_list_end = .;
        KEEP(*(.objc_selectors))
        KEEP(*(.objc_classes))
        KEEP(*(.objc_category))
        KEEP(*(.objc_class_refs))
        KEEP(*(.objc_class_names))
        KEEP(*(.objc_method_names))
        KEEP(*(.objc_method_types))
        KEEP(*(.objc_protocol_list))
        KEEP(*(.objc_string_object))
        KEEP(*(.objc_constant_string))
        KEEP(*(.objc_data))
        KEEP(*(.objc_list.*))
        
        __objc_runtime_end = .;
    }
}
INSERT AFTER .data;