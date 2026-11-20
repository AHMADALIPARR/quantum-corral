CC      := gcc
CFLAGS  := -std=c11 -O2 -Wall -Wextra -Inative/include
LDFLAGS := -lm

TINYAPL_SRCS := \
	tinyapl/array_ir/array_ir.c \
	tinyapl/parser/tinyapl_parser.c \
	tinyapl/typecheck/typecheck.c \
	tinyapl/lowering/c11/c11_lowering.c \
	tinyapl/tinyaplc.c

RUNTIME_SRCS := \
	native/runtime/corral_runtime.c \
	native/bridge/wstp_adapter.c \
	native/bridge/matlab_adapter.c \
	vulkan/compute/vk_corral_compute.c \
	vulkan/renderer/vk_corral_renderer.c

.PHONY: all test clean

all: tinyaplc verify_abi verify_e2e

tinyaplc: $(TINYAPL_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

generated_corral.c: tinyaplc verification/array_semantics/test_tinyapl_lowering.apl
	./tinyaplc verification/array_semantics/test_tinyapl_lowering.apl -o $@

verify_abi: verification/abi/test_abi_layout.c $(RUNTIME_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

verify_e2e: verification/numerical/test_corral_e2e.c generated_corral.c $(RUNTIME_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test: all
	./verify_abi
	./verify_e2e

clean:
	rm -f tinyaplc verify_abi verify_e2e generated_corral.c corral_frame.ppm
