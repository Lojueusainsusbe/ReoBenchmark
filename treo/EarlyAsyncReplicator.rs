// Generated by Reo 1.0

#[allow(unused_imports)]
use maplit::{hashmap, hashset};

pub use reo_rs::*;
use reo_rs::building::{*, NameDef::*};
use reo_rs::{Instruction::*, Term::*};

#[no_mangle]
pub extern fn reors_generated_proto_create() -> CProtoHandle {
    reo_rs::to_c_proto(proto_protocol1_build_rust::<isize>())
}

pub fn proto_protocol1_build_rust<T0>(
) -> ProtoHandle
where
    T0: 'static + Send + Sync + Sized,
{
    let name_defs = hashmap!{
        "p_1" => Port { is_putter:  true, type_info: TypeInfo::of::<T0>() },
        "x_1" => Port { is_putter: false, type_info: TypeInfo::of::<T0>() },
        "m1" => Mem(TypeInfo::of::<T0>()),
    };
    let rules = vec![
        RuleDef {
            state_guard: StatePredicate {
                ready_ports: hashset! {"p_1", },
                full_mem: hashset! {},
                empty_mem: hashset! {"m1", },
            },
            ins: vec![
            ],
            output: hashmap!{
                "p_1" => (false, hashset!{"m1", }),
            }
        },
        RuleDef {
            state_guard: StatePredicate {
                ready_ports: hashset! {"x_1", },
                full_mem: hashset! {"m1", },
                empty_mem: hashset! {},
            },
            ins: vec![
            ],
            output: hashmap!{
                "m1" => (false, hashset!{"x_1", }),
            }
        },
    ];
    let mem_init = MemInitial::default();
    ProtoDef {
        name_defs,
        rules
    }.build(mem_init).expect("Oh no! Reo failed to build!")
}

