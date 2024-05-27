---
layout: post
title: "My Experiments With JAX"
excerpt: "If TensorFlow and Haskell had a child ... "
tags: [JAX, Pytorch, Deep Learning, ML, Machine Learning, Compilers, ML Compilers]
comments: true
---

Hello Folks,

I am back after a <ahem> short break. So let's dive in.
I have spent a bit building / playing with MLIR based ML Compilers. I will
chronicle a bit of my experience with JAX as a framework. JAX is regarded as the
framework of AGI.

Below are the raw instructions, copied from Jupyter Notebook where I ran the experiments.

```python
!pip install -qqq jaxtyping hypothesis pytest penzai

```


```python
import jax.numpy as np
import numpy as onp
from penzai import pz
arange = pz.nx.arange
where = pz.nx.nmap(np.where)
wrap = pz.nx.wrap
pz.ts.register_as_default()

# Optional automatic array visualization extras:
pz.ts.register_autovisualize_magic()
pz.enable_interactive_context()
pz.ts.active_autovisualizer.set_interactive(pz.ts.ArrayAutovisualizer(force_continuous=True, around_zero=True,  prefers_column=["j"], prefers_row=["i"]))
```


```python
import os
# GPU flags
flags = (
    "--xla_gpu_enable_triton_softmax_fusion=true "
    "--xla_gpu_triton_gemm_any=false "
    "--xla_gpu_enable_async_collectives=true "
    "--xla_gpu_enable_latency_hiding_scheduler=true "
    "--xla_gpu_enable_highest_priority_async_stream=true "
)
os.environ["XLA_FLAGS"] = flags
```


```python
!nvidia-smi
```

    Mon May 27 13:31:30 2024       
    +---------------------------------------------------------------------------------------+
    | NVIDIA-SMI 545.29.06              Driver Version: 545.29.06    CUDA Version: 12.3     |
    |-----------------------------------------+----------------------+----------------------+
    | GPU  Name                 Persistence-M | Bus-Id        Disp.A | Volatile Uncorr. ECC |
    | Fan  Temp   Perf          Pwr:Usage/Cap |         Memory-Usage | GPU-Util  Compute M. |
    |                                         |                      |               MIG M. |
    |=========================================+======================+======================|
    |   0  NVIDIA GeForce RTX 4090        Off | 00000000:09:00.0  On |                  Off |
    |  0%   47C    P2              58W / 450W |  19106MiB / 24564MiB |      1%      Default |
    |                                         |                      |                  N/A |
    +-----------------------------------------+----------------------+----------------------+
                                                                                             
    +---------------------------------------------------------------------------------------+
    | Processes:                                                                            |
    |  GPU   GI   CI        PID   Type   Process name                            GPU Memory |
    |        ID   ID                                                             Usage      |
    |=======================================================================================|
    |    0   N/A  N/A      1195      G   /usr/lib/xorg/Xorg                          259MiB |
    |    0   N/A  N/A      2130      G   /usr/lib/firefox/firefox                      0MiB |
    |    0   N/A  N/A    477865      C   /home/mycpuorg/miniconda3/bin/python3.11     18404MiB |
    +---------------------------------------------------------------------------------------+


    /home/mycpuorg/miniconda3/lib/python3.11/pty.py:89: RuntimeWarning: os.fork() was called. os.fork() is incompatible with multithreaded code, and JAX is multithreaded, so this will likely lead to a deadlock.
      pid, fd = os.forkpty()



```python
import jax
import jax.numpy as jnp

# Define a simple function - test
def test(x):
  return jnp.sin(x) * x**2


# Pretty-print the function as a Jaxpr
print(jax.make_jaxpr(test)(1.0))
```

    { lambda ; a:f32[]. let
        b:f32[] = sin a
        c:f32[] = integer_pow[y=2] a
        d:f32[] = mul b c
      in (d,) }



```python


# Lower the function
lowered = jax.jit(test).lower(1.0)

# Print the lowered IR as text
print(lowered.as_text())

# Get the StableHLO representation of the lowered IR 
print(lowered.compiler_ir(dialect="stablehlo"))

```

    module @jit_test attributes {mhlo.num_partitions = 1 : i32, mhlo.num_replicas = 1 : i32} {
      func.func public @main(%arg0: tensor<f32> {mhlo.layout_mode = "default"}) -> (tensor<f32> {jax.result_info = "", mhlo.layout_mode = "default"}) {
        %0 = stablehlo.sine %arg0 : tensor<f32>
        %1 = stablehlo.multiply %arg0, %arg0 : tensor<f32>
        %2 = stablehlo.multiply %0, %1 : tensor<f32>
        return %2 : tensor<f32>
      }
    }
    
    module @jit_test attributes {mhlo.num_partitions = 1 : i32, mhlo.num_replicas = 1 : i32} {
      func.func public @main(%arg0: tensor<f32> {mhlo.layout_mode = "default"}) -> (tensor<f32> {jax.result_info = "", mhlo.layout_mode = "default"}) {
        %0 = stablehlo.sine %arg0 : tensor<f32>
        %1 = stablehlo.multiply %arg0, %arg0 : tensor<f32>
        %2 = stablehlo.multiply %0, %1 : tensor<f32>
        return %2 : tensor<f32>
      }
    }
    



```python

# Compile the function
compiled = lowered.compile()

# Print the compiled IR as text
print(compiled.as_text())

# Get a summary of execution costs
print(compiled.cost_analysis())

```

    HloModule jit_test, is_scheduled=true, entry_computation_layout={(f32[])->f32[]}, allow_spmd_sharding_propagation_to_parameters={true}, allow_spmd_sharding_propagation_to_output={true}, frontend_attributes={fingerprint_before_lhs="9aa3c44fdb740a754eb15d47d89c333e"}
    
    %fused_multiply (param_0.2: f32[]) -> f32[] {
      %param_0.2 = f32[] parameter(0)
      %sine.1.1 = f32[] sine(f32[] %param_0.2), metadata={op_name="jit(test)/jit(main)/sin" source_file="/tmp/ipykernel_477865/1530176408.py" source_line=6}
      %multiply.2.1 = f32[] multiply(f32[] %param_0.2, f32[] %param_0.2), metadata={op_name="jit(test)/jit(main)/integer_pow[y=2]" source_file="/tmp/ipykernel_477865/1530176408.py" source_line=6}
      ROOT %multiply.5.1 = f32[] multiply(f32[] %sine.1.1, f32[] %multiply.2.1), metadata={op_name="jit(test)/jit(main)/mul" source_file="/tmp/ipykernel_477865/1530176408.py" source_line=6}
    }
    
    ENTRY %main.5 (Arg_0.1.0: f32[]) -> f32[] {
      %Arg_0.1.0 = f32[] parameter(0)
      ROOT %loop_multiply_fusion = f32[] fusion(f32[] %Arg_0.1.0), kind=kLoop, calls=%fused_multiply, metadata={op_name="jit(test)/jit(main)/mul" source_file="/tmp/ipykernel_477865/1530176408.py" source_line=6}
    }
    
    
    [{'utilization0{}': 1.0, 'bytes accessed1{}': 8.0, 'bytes accessedout{}': 4.0, 'bytes accessed': 8.0, 'transcendentals': 1.0, 'bytes accessed0{}': 4.0, 'utilization1{}': 2.0, 'flops': 2.0}]



```python
import jax.numpy as jnp
```


```python
from jax import grad, jit, vmap
from jax import random
```


```python
# A helper function to randomly initialize weights and biases
# for dense neural network layer

def random_layer_params(m, n, key, scale=1e-2):
    w_key, b_key = random.split(key)
    return scale * random.normal(w_key, (n,m)), scale * random.normal(b_key, (n,))

# Intialize all layers for a fully-connected neural network with sizes "sizes"
def init_network_params(sizes, key):
    keys = random.split(key, len(sizes))
    return [random_layer_params(m, n, k) for m, n, k in zip(sizes[:-1], sizes[1:], keys)]

layer_sizes = [784, 512, 512, 10]
step_size = 0.01
num_epochs = 10
batch_size = 128
n_targets = 10
params = init_network_params(layer_sizes, random.key(0))
```


```python
from jax.scipy.special import logsumexp

def relu(x):
    return jnp.maximum(0, x)

def predict(params, image):
    # per-example predictions
    activations = image
    for w, b in params[:-1]:
        outputs = jnp.dot(w, activations) + b
        activations = relu(outputs)

    final_w, final_b = params[-1]
    logits = jnp.dot(final_w, activations) + final_b
    return logits - logsumexp(logits)

```


```python
random_flattened_image = random.normal(random.key(1), (28*28,))
print(random_flattened_image.shape)
print(len(params))
preds = predict(params, random_flattened_image)
print(preds.shape)
```

    (784,)
    3
    (10,)



```python
batched_random_flattened_images = random.normal(random.key(1), (10, 28 * 28))
try:
    preds = predict(params, batched_random_flattened_images)
except TypeError:
    print("Invalid Shapes!")

```

    Invalid Shapes!



```python
batched_predict = vmap(predict, in_axes=(None, 0))
batched_preds = batched_predict(params, batched_random_flattened_images)
print(batched_preds.shape)
```

    (10, 10)


## Loss Function and other utils


```python
def one_hot(x, k, dtype=jnp.float32):
    return jnp.array(x[:, None] == jnp.arange(k), dtype)

def accuracy(params, images, targets):
    target_class = jnp.argmax(targets, axis=1)
    predicted_class = jnp.argmax(batched_predict(params, images), axis=1)
    return jnp.mean(predicted_class == target_class)

def loss(params, images, targets):
    preds = batched_predict(params, images)
    return -(jnp.mean(preds * targets))

@jit
def update(params, x, y):
    grads = grad(loss)(params, x, y)
    return [(w - step_size * dw, b - step_size * db) for (w, b), (dw, db) in zip(params, grads)]


```


```python

```


```python
import numpy as np
from jax.tree_util import tree_map
from torchvision.datasets import MNIST
from torch.utils import data
```

    /home/mycpuorg/miniconda3/lib/python3.11/site-packages/torchvision/io/image.py:13: UserWarning: Failed to load image Python extension: '/home/mycpuorg/miniconda3/lib/python3.11/site-packages/torchvision/image.so: undefined symbol: _ZN3c1017RegisterOperatorsD1Ev'If you don't plan on using image functionality from `torchvision.io`, you can ignore this warning. Otherwise, there might be something wrong with your environment. Did you have `libjpeg` or `libpng` installed before building `torchvision` from source?
      warn(
    /home/mycpuorg/miniconda3/lib/python3.11/site-packages/tqdm/auto.py:21: TqdmWarning: IProgress not found. Please update jupyter and ipywidgets. See https://ipywidgets.readthedocs.io/en/stable/user_install.html
      from .autonotebook import tqdm as notebook_tqdm



```python
def numpy_collate(batch):
    return tree_map(np.asarray, data.default_collate(batch))

class NumpyLoader(data.DataLoader):
    def __init__(self, dataset, batch_size=1,
                 shuffle=False, sampler=None,
                 batch_sampler=None, num_workers=0,
                 pin_memory=False, drop_last=False,
                 timeout=0, worker_init_fn=None):
        super(self.__class__, self).__init__(dataset,
             batch_size=batch_size,
             shuffle=shuffle,
             sampler=sampler,
             batch_sampler=batch_sampler,
             num_workers=num_workers,
             collate_fn=numpy_collate,
             pin_memory=pin_memory,
             drop_last=drop_last,
             timeout=timeout,
             worker_init_fn=worker_init_fn)
    
class FlattenAndCast(object):
    def __call__(self, pic):
        return np.ravel(np.array(pic, dtype=jnp.float32))
            

             
```


```python
# Define our dataset, using torch datasets
mnist_dataset = MNIST('/tmp/mnist/', download=True, transform=FlattenAndCast())
training_generator = NumpyLoader(mnist_dataset, batch_size=batch_size, num_workers=0)
```


```python
a = np.array(np.random.randn(3,2,4))
a.reshape(-1, 24)

b = jnp.array(a)
print(b.devices())
b.sharding

x = jnp.arange(5)
w = jnp.array([2., 3., 4.])

def convolve(x ,w):
    output = []
    for i in range(1, len(x) - 1):
        output.append(jnp.dot(x[i - 1 : i + 2], w))
    return jnp.array(output)

lowered_conv = jax.jit(convolve).lower(x, w)
print(lowered_conv.compiler_ir(dialect="stablehlo"))
# print(lowered_conv.compiler_ir(dialect="mhlo"))

# print(lowered_conv.compiler_ir(dialect="LMHLO"))

compiled_conv = lowered_conv.compile()
print(compiled_conv.as_text())
ca = compiled_conv.cost_analysis()
ca
```

    {cuda(id=0)}
    module @jit_convolve attributes {mhlo.num_partitions = 1 : i32, mhlo.num_replicas = 1 : i32} {
      func.func public @main(%arg0: tensor<5xi32> {mhlo.layout_mode = "default"}, %arg1: tensor<3xf32> {mhlo.layout_mode = "default"}) -> (tensor<3xf32> {jax.result_info = "", mhlo.layout_mode = "default"}) {
        %0 = stablehlo.slice %arg0 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %1 = stablehlo.convert %0 : (tensor<3xi32>) -> tensor<3xf32>
        %2 = stablehlo.convert %arg1 : tensor<3xf32>
        %3 = stablehlo.dot_general %1, %2, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %4 = stablehlo.slice %arg0 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %5 = stablehlo.convert %4 : (tensor<3xi32>) -> tensor<3xf32>
        %6 = stablehlo.convert %arg1 : tensor<3xf32>
        %7 = stablehlo.dot_general %5, %6, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %8 = stablehlo.slice %arg0 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %9 = stablehlo.convert %8 : (tensor<3xi32>) -> tensor<3xf32>
        %10 = stablehlo.convert %arg1 : tensor<3xf32>
        %11 = stablehlo.dot_general %9, %10, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %12 = stablehlo.broadcast_in_dim %3, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %13 = stablehlo.broadcast_in_dim %7, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %14 = stablehlo.broadcast_in_dim %11, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %15 = stablehlo.concatenate %12, %13, %14, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        return %15 : tensor<3xf32>
      }
    }
    
    HloModule jit_convolve, is_scheduled=true, entry_computation_layout={(s32[5]{0}, f32[3]{0})->f32[3]{0}}, allow_spmd_sharding_propagation_to_parameters={true,true}, allow_spmd_sharding_propagation_to_output={true}, frontend_attributes={fingerprint_before_lhs="f99fa116847aaf794a773015b66e3c7a"}
    
    %scalar_add_computation (scalar_lhs: f32[], scalar_rhs: f32[]) -> f32[] {
      %scalar_rhs = f32[] parameter(1)
      %scalar_lhs = f32[] parameter(0)
      ROOT %add.2 = f32[] add(f32[] %scalar_lhs, f32[] %scalar_rhs)
    }
    
    %fused_concatenate (param_0.15: f32[3], param_1.19: s32[5]) -> f32[3] {
      %param_1.19 = s32[5]{0} parameter(1)
      %convert.2.5 = f32[5]{0} convert(s32[5]{0} %param_1.19), metadata={op_name="jit(convolve)/jit(main)/dot_general[dimension_numbers=(((0,), (0,)), ((), ())) precision=None preferred_element_type=float32]" source_file="/tmp/ipykernel_477865/1956054057.py" source_line=14}
      %slice.8.3 = f32[3]{0} slice(f32[5]{0} %convert.2.5), slice={[0:3]}, metadata={op_name="jit(convolve)/jit(main)/slice[start_indices=(0,) limit_indices=(3,) strides=None]" source_file="/tmp/ipykernel_477865/1956054057.py" source_line=14}
      %param_0.15 = f32[3]{0} parameter(0)
      %multiply.6.3 = f32[3]{0} multiply(f32[3]{0} %slice.8.3, f32[3]{0} %param_0.15)
      %constant_7 = f32[] constant(0)
      %reduce.4 = f32[] reduce(f32[3]{0} %multiply.6.3, f32[] %constant_7), dimensions={0}, to_apply=%scalar_add_computation, metadata={op_name="jit(convolve)/jit(main)/dot_general[dimension_numbers=(((0,), (0,)), ((), ())) precision=None preferred_element_type=float32]" source_file="/tmp/ipykernel_477865/1956054057.py" source_line=14}
      %bitcast.63.1 = f32[1]{0} bitcast(f32[] %reduce.4)
      %slice.10.3 = f32[3]{0} slice(f32[5]{0} %convert.2.5), slice={[1:4]}, metadata={op_name="jit(convolve)/jit(main)/slice[start_indices=(1,) limit_indices=(4,) strides=None]" source_file="/tmp/ipykernel_477865/1956054057.py" source_line=14}
      %multiply.7.3 = f32[3]{0} multiply(f32[3]{0} %slice.10.3, f32[3]{0} %param_0.15)
      %reduce.1.1 = f32[] reduce(f32[3]{0} %multiply.7.3, f32[] %constant_7), dimensions={0}, to_apply=%scalar_add_computation, metadata={op_name="jit(convolve)/jit(main)/dot_general[dimension_numbers=(((0,), (0,)), ((), ())) precision=None preferred_element_type=float32]" source_file="/tmp/ipykernel_477865/1956054057.py" source_line=14}
      %bitcast.75.1 = f32[1]{0} bitcast(f32[] %reduce.1.1)
      %slice.11.3 = f32[3]{0} slice(f32[5]{0} %convert.2.5), slice={[2:5]}, metadata={op_name="jit(convolve)/jit(main)/slice[start_indices=(2,) limit_indices=(5,) strides=None]" source_file="/tmp/ipykernel_477865/1956054057.py" source_line=14}
      %multiply.8.3 = f32[3]{0} multiply(f32[3]{0} %slice.11.3, f32[3]{0} %param_0.15)
      %reduce.2.1 = f32[] reduce(f32[3]{0} %multiply.8.3, f32[] %constant_7), dimensions={0}, to_apply=%scalar_add_computation, metadata={op_name="jit(convolve)/jit(main)/dot_general[dimension_numbers=(((0,), (0,)), ((), ())) precision=None preferred_element_type=float32]" source_file="/tmp/ipykernel_477865/1956054057.py" source_line=14}
      %bitcast.87.1 = f32[1]{0} bitcast(f32[] %reduce.2.1)
      ROOT %concatenate.1.1 = f32[3]{0} concatenate(f32[1]{0} %bitcast.63.1, f32[1]{0} %bitcast.75.1, f32[1]{0} %bitcast.87.1), dimensions={0}, metadata={op_name="jit(convolve)/jit(main)/concatenate[dimension=0]" source_file="/tmp/ipykernel_477865/1956054057.py" source_line=15}
    }
    
    ENTRY %main.16 (Arg_0.1.0: s32[5], Arg_1.2.0: f32[3]) -> f32[3] {
      %Arg_1.2.0 = f32[3]{0} parameter(1)
      %Arg_0.1.0 = s32[5]{0} parameter(0)
      ROOT %input_concatenate_fusion = f32[3]{0} fusion(f32[3]{0} %Arg_1.2.0, s32[5]{0} %Arg_0.1.0), kind=kInput, calls=%fused_concatenate, metadata={op_name="jit(convolve)/jit(main)/concatenate[dimension=0]" source_file="/tmp/ipykernel_477865/1956054057.py" source_line=15}
    }
    
    



<div id="output_16fd85c31031421091d554d2b618d00b"><script> /* penzai.treescope rendering of a Python object (compressed) */ (()=>{ let observer; let lastStep = new Promise((resolve, reject) => { observer = new IntersectionObserver((entries) => { for (const entry of entries) { if (entry.isIntersecting) { resolve(); observer.disconnect(); return; } } }, {rootMargin: "1000px"}); }); window.treescope_decompress_enqueue = (encoded, destId) => { const previous = lastStep; const destElt = document.getElementById(destId); lastStep = (async () => { await previous; let blob = new Blob([ Uint8Array.from(atob(encoded), (m) => m.codePointAt(0)) ]); let reader = blob.stream().pipeThrough( new DecompressionStream("deflate") ).pipeThrough( new TextDecoderStream("utf-8") ).getReader(); let parts = []; while (true) { let step = await reader.read(); if (step.done) { break; } parts.push(step.value); } let newElt = document.createElement("div"); newElt.innerHTML = parts.join(""); destElt.parentNode.replaceChild(newElt, destElt); for (let oldScript of newElt.querySelectorAll("script")) { let newScript = document.createElement("script"); newScript.type = oldScript.type; newScript.textContent = oldScript.textContent; oldScript.parentNode.replaceChild(newScript, oldScript); } })(); requestAnimationFrame(() => { observer.observe(destElt); }); } })(); </script><div id="compress_html_1dbce3bdd7654320b051ead53afc7a99"><script>window.treescope_decompress_enqueue("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", "compress_html_1dbce3bdd7654320b051ead53afc7a99");</script><span style="color: #aaaaaa; font-family: monospace">(Loading...)</span></div></div>





<div id="output_dest_16fd85c31031421091d554d2b618d00b"><script> (()=>{ const output = document.getElementById("output_16fd85c31031421091d554d2b618d00b"); const dest = document.getElementById("output_dest_16fd85c31031421091d554d2b618d00b"); dest.parentNode.replaceChild(output, dest); })(); </script></div>




```python
xs = jnp.stack([x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x])
ws = jnp.stack([w, w, w, w, w, w, w, w, w, w, w, w, w, w, w, w])
```


```python
def manual_batched_conv(xs, ws):
    output = []
    for i in range(xs.shape[0]):
        output.append(convolve(xs[i], ws[i]))
    return jnp.stack(output)

manual_batched_conv(xs, ws)
```


<div id="output_0a511e22e04848c6be0197f638ab1cdb"><script> /* penzai.treescope rendering of a Python object (compressed) */ (()=>{ let observer; let lastStep = new Promise((resolve, reject) => { observer = new IntersectionObserver((entries) => { for (const entry of entries) { if (entry.isIntersecting) { resolve(); observer.disconnect(); return; } } }, {rootMargin: "1000px"}); }); window.treescope_decompress_enqueue = (encoded, destId) => { const previous = lastStep; const destElt = document.getElementById(destId); lastStep = (async () => { await previous; let blob = new Blob([ Uint8Array.from(atob(encoded), (m) => m.codePointAt(0)) ]); let reader = blob.stream().pipeThrough( new DecompressionStream("deflate") ).pipeThrough( new TextDecoderStream("utf-8") ).getReader(); let parts = []; while (true) { let step = await reader.read(); if (step.done) { break; } parts.push(step.value); } let newElt = document.createElement("div"); newElt.innerHTML = parts.join(""); destElt.parentNode.replaceChild(newElt, destElt); for (let oldScript of newElt.querySelectorAll("script")) { let newScript = document.createElement("script"); newScript.type = oldScript.type; newScript.textContent = oldScript.textContent; oldScript.parentNode.replaceChild(newScript, oldScript); } })(); requestAnimationFrame(() => { observer.observe(destElt); }); } })(); </script><div id="compress_html_30b2f3be8270402b9df15b320e7b32d5"><script>window.treescope_decompress_enqueue("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", "compress_html_30b2f3be8270402b9df15b320e7b32d5");</script><span style="color: #aaaaaa; font-family: monospace">(Loading...)</span></div></div>



<div id="compress_html_3de02e1c55e44ab79c8cdc063fdd63bc"><script>window.treescope_decompress_enqueue("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", "compress_html_3de02e1c55e44ab79c8cdc063fdd63bc");</script></div>





<div id="output_dest_0a511e22e04848c6be0197f638ab1cdb"><script> (()=>{ const output = document.getElementById("output_0a511e22e04848c6be0197f638ab1cdb"); const dest = document.getElementById("output_dest_0a511e22e04848c6be0197f638ab1cdb"); dest.parentNode.replaceChild(output, dest); })(); </script></div>




```python
auto_batch_conv = jax.vmap(convolve)

auto_batch_conv(xs, ws)
```


<div id="output_df952ef160c347d588ec7f497c6cf1ff"><script> /* penzai.treescope rendering of a Python object (compressed) */ (()=>{ let observer; let lastStep = new Promise((resolve, reject) => { observer = new IntersectionObserver((entries) => { for (const entry of entries) { if (entry.isIntersecting) { resolve(); observer.disconnect(); return; } } }, {rootMargin: "1000px"}); }); window.treescope_decompress_enqueue = (encoded, destId) => { const previous = lastStep; const destElt = document.getElementById(destId); lastStep = (async () => { await previous; let blob = new Blob([ Uint8Array.from(atob(encoded), (m) => m.codePointAt(0)) ]); let reader = blob.stream().pipeThrough( new DecompressionStream("deflate") ).pipeThrough( new TextDecoderStream("utf-8") ).getReader(); let parts = []; while (true) { let step = await reader.read(); if (step.done) { break; } parts.push(step.value); } let newElt = document.createElement("div"); newElt.innerHTML = parts.join(""); destElt.parentNode.replaceChild(newElt, destElt); for (let oldScript of newElt.querySelectorAll("script")) { let newScript = document.createElement("script"); newScript.type = oldScript.type; newScript.textContent = oldScript.textContent; oldScript.parentNode.replaceChild(newScript, oldScript); } })(); requestAnimationFrame(() => { observer.observe(destElt); }); } })(); </script><div id="compress_html_bdb81c4942ea4876972261691b09562e"><script>window.treescope_decompress_enqueue("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", "compress_html_bdb81c4942ea4876972261691b09562e");</script><span style="color: #aaaaaa; font-family: monospace">(Loading...)</span></div></div>



<div id="compress_html_042acebae34b4563b362aaeaa1df8fb4"><script>window.treescope_decompress_enqueue("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", "compress_html_042acebae34b4563b362aaeaa1df8fb4");</script></div>





<div id="output_dest_df952ef160c347d588ec7f497c6cf1ff"><script> (()=>{ const output = document.getElementById("output_df952ef160c347d588ec7f497c6cf1ff"); const dest = document.getElementById("output_dest_df952ef160c347d588ec7f497c6cf1ff"); dest.parentNode.replaceChild(output, dest); })(); </script></div>




```python
lowered_batch_conv = jax.jit(auto_batch_conv, device=jax.devices()[0]).lower(xs, ws)
compiled_auto_batch_conv = lowered_batch_conv.compile()
print(compiled_auto_batch_conv.cost_analysis())
print(lowered_batch_conv.compiler_ir())
```

    [{'bytes accessed': 704.0, 'bytes accessed0{}': 192.0, 'utilization1{}': 1.0, 'utilization2{}': 1.0, 'bytes accessed1{}': 320.0, 'bytes accessedout{}': 192.0, 'utilization0{}': 1.0, 'flops': 320.0, 'bytes accessed2{}': 64.0}]
    module @jit_convolve attributes {mhlo.num_partitions = 1 : i32, mhlo.num_replicas = 1 : i32} {
      func.func public @main(%arg0: tensor<16x5xi32> {mhlo.layout_mode = "default", mhlo.sharding = "{replicated}"}, %arg1: tensor<16x3xf32> {mhlo.layout_mode = "default", mhlo.sharding = "{replicated}"}) -> (tensor<16x3xf32> {jax.result_info = "", mhlo.layout_mode = "default", mhlo.sharding = "{replicated}"}) {
        %0 = stablehlo.slice %arg0 [0:16, 0:3] : (tensor<16x5xi32>) -> tensor<16x3xi32>
        %1 = stablehlo.convert %0 : (tensor<16x3xi32>) -> tensor<16x3xf32>
        %2 = stablehlo.convert %arg1 : tensor<16x3xf32>
        %3 = stablehlo.dot_general %1, %2, batching_dims = [0] x [0], contracting_dims = [1] x [1], precision = [DEFAULT, DEFAULT] : (tensor<16x3xf32>, tensor<16x3xf32>) -> tensor<16xf32>
        %4 = stablehlo.slice %arg0 [0:16, 1:4] : (tensor<16x5xi32>) -> tensor<16x3xi32>
        %5 = stablehlo.convert %4 : (tensor<16x3xi32>) -> tensor<16x3xf32>
        %6 = stablehlo.convert %arg1 : tensor<16x3xf32>
        %7 = stablehlo.dot_general %5, %6, batching_dims = [0] x [0], contracting_dims = [1] x [1], precision = [DEFAULT, DEFAULT] : (tensor<16x3xf32>, tensor<16x3xf32>) -> tensor<16xf32>
        %8 = stablehlo.slice %arg0 [0:16, 2:5] : (tensor<16x5xi32>) -> tensor<16x3xi32>
        %9 = stablehlo.convert %8 : (tensor<16x3xi32>) -> tensor<16x3xf32>
        %10 = stablehlo.convert %arg1 : tensor<16x3xf32>
        %11 = stablehlo.dot_general %9, %10, batching_dims = [0] x [0], contracting_dims = [1] x [1], precision = [DEFAULT, DEFAULT] : (tensor<16x3xf32>, tensor<16x3xf32>) -> tensor<16xf32>
        %12 = stablehlo.broadcast_in_dim %3, dims = [0] : (tensor<16xf32>) -> tensor<16x1xf32>
        %13 = stablehlo.broadcast_in_dim %7, dims = [0] : (tensor<16xf32>) -> tensor<16x1xf32>
        %14 = stablehlo.broadcast_in_dim %11, dims = [0] : (tensor<16xf32>) -> tensor<16x1xf32>
        %15 = stablehlo.concatenate %12, %13, %14, dim = 1 : (tensor<16x1xf32>, tensor<16x1xf32>, tensor<16x1xf32>) -> tensor<16x3xf32>
        return %15 : tensor<16x3xf32>
      }
    }
    



```python
lowered_manual_batch_conv = jax.jit(manual_batched_conv, device=jax.devices()[0]).lower(xs, ws)
print(lowered_manual_batch_conv.compiler_ir())
```

    module @jit_manual_batched_conv attributes {mhlo.num_partitions = 1 : i32, mhlo.num_replicas = 1 : i32} {
      func.func public @main(%arg0: tensor<16x5xi32> {mhlo.layout_mode = "default", mhlo.sharding = "{replicated}"}, %arg1: tensor<16x3xf32> {mhlo.layout_mode = "default", mhlo.sharding = "{replicated}"}) -> (tensor<16x3xf32> {jax.result_info = "", mhlo.layout_mode = "default", mhlo.sharding = "{replicated}"}) {
        %0 = stablehlo.slice %arg0 [0:1, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %1 = stablehlo.reshape %0 : (tensor<1x5xi32>) -> tensor<5xi32>
        %2 = stablehlo.slice %arg1 [0:1, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %3 = stablehlo.reshape %2 : (tensor<1x3xf32>) -> tensor<3xf32>
        %4 = stablehlo.slice %1 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %5 = stablehlo.convert %4 : (tensor<3xi32>) -> tensor<3xf32>
        %6 = stablehlo.convert %3 : tensor<3xf32>
        %7 = stablehlo.dot_general %5, %6, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %8 = stablehlo.slice %1 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %9 = stablehlo.convert %8 : (tensor<3xi32>) -> tensor<3xf32>
        %10 = stablehlo.convert %3 : tensor<3xf32>
        %11 = stablehlo.dot_general %9, %10, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %12 = stablehlo.slice %1 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %13 = stablehlo.convert %12 : (tensor<3xi32>) -> tensor<3xf32>
        %14 = stablehlo.convert %3 : tensor<3xf32>
        %15 = stablehlo.dot_general %13, %14, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %16 = stablehlo.broadcast_in_dim %7, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %17 = stablehlo.broadcast_in_dim %11, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %18 = stablehlo.broadcast_in_dim %15, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %19 = stablehlo.concatenate %16, %17, %18, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %20 = stablehlo.slice %arg0 [1:2, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %21 = stablehlo.reshape %20 : (tensor<1x5xi32>) -> tensor<5xi32>
        %22 = stablehlo.slice %arg1 [1:2, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %23 = stablehlo.reshape %22 : (tensor<1x3xf32>) -> tensor<3xf32>
        %24 = stablehlo.slice %21 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %25 = stablehlo.convert %24 : (tensor<3xi32>) -> tensor<3xf32>
        %26 = stablehlo.convert %23 : tensor<3xf32>
        %27 = stablehlo.dot_general %25, %26, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %28 = stablehlo.slice %21 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %29 = stablehlo.convert %28 : (tensor<3xi32>) -> tensor<3xf32>
        %30 = stablehlo.convert %23 : tensor<3xf32>
        %31 = stablehlo.dot_general %29, %30, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %32 = stablehlo.slice %21 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %33 = stablehlo.convert %32 : (tensor<3xi32>) -> tensor<3xf32>
        %34 = stablehlo.convert %23 : tensor<3xf32>
        %35 = stablehlo.dot_general %33, %34, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %36 = stablehlo.broadcast_in_dim %27, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %37 = stablehlo.broadcast_in_dim %31, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %38 = stablehlo.broadcast_in_dim %35, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %39 = stablehlo.concatenate %36, %37, %38, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %40 = stablehlo.slice %arg0 [2:3, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %41 = stablehlo.reshape %40 : (tensor<1x5xi32>) -> tensor<5xi32>
        %42 = stablehlo.slice %arg1 [2:3, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %43 = stablehlo.reshape %42 : (tensor<1x3xf32>) -> tensor<3xf32>
        %44 = stablehlo.slice %41 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %45 = stablehlo.convert %44 : (tensor<3xi32>) -> tensor<3xf32>
        %46 = stablehlo.convert %43 : tensor<3xf32>
        %47 = stablehlo.dot_general %45, %46, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %48 = stablehlo.slice %41 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %49 = stablehlo.convert %48 : (tensor<3xi32>) -> tensor<3xf32>
        %50 = stablehlo.convert %43 : tensor<3xf32>
        %51 = stablehlo.dot_general %49, %50, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %52 = stablehlo.slice %41 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %53 = stablehlo.convert %52 : (tensor<3xi32>) -> tensor<3xf32>
        %54 = stablehlo.convert %43 : tensor<3xf32>
        %55 = stablehlo.dot_general %53, %54, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %56 = stablehlo.broadcast_in_dim %47, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %57 = stablehlo.broadcast_in_dim %51, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %58 = stablehlo.broadcast_in_dim %55, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %59 = stablehlo.concatenate %56, %57, %58, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %60 = stablehlo.slice %arg0 [3:4, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %61 = stablehlo.reshape %60 : (tensor<1x5xi32>) -> tensor<5xi32>
        %62 = stablehlo.slice %arg1 [3:4, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %63 = stablehlo.reshape %62 : (tensor<1x3xf32>) -> tensor<3xf32>
        %64 = stablehlo.slice %61 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %65 = stablehlo.convert %64 : (tensor<3xi32>) -> tensor<3xf32>
        %66 = stablehlo.convert %63 : tensor<3xf32>
        %67 = stablehlo.dot_general %65, %66, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %68 = stablehlo.slice %61 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %69 = stablehlo.convert %68 : (tensor<3xi32>) -> tensor<3xf32>
        %70 = stablehlo.convert %63 : tensor<3xf32>
        %71 = stablehlo.dot_general %69, %70, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %72 = stablehlo.slice %61 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %73 = stablehlo.convert %72 : (tensor<3xi32>) -> tensor<3xf32>
        %74 = stablehlo.convert %63 : tensor<3xf32>
        %75 = stablehlo.dot_general %73, %74, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %76 = stablehlo.broadcast_in_dim %67, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %77 = stablehlo.broadcast_in_dim %71, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %78 = stablehlo.broadcast_in_dim %75, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %79 = stablehlo.concatenate %76, %77, %78, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %80 = stablehlo.slice %arg0 [4:5, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %81 = stablehlo.reshape %80 : (tensor<1x5xi32>) -> tensor<5xi32>
        %82 = stablehlo.slice %arg1 [4:5, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %83 = stablehlo.reshape %82 : (tensor<1x3xf32>) -> tensor<3xf32>
        %84 = stablehlo.slice %81 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %85 = stablehlo.convert %84 : (tensor<3xi32>) -> tensor<3xf32>
        %86 = stablehlo.convert %83 : tensor<3xf32>
        %87 = stablehlo.dot_general %85, %86, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %88 = stablehlo.slice %81 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %89 = stablehlo.convert %88 : (tensor<3xi32>) -> tensor<3xf32>
        %90 = stablehlo.convert %83 : tensor<3xf32>
        %91 = stablehlo.dot_general %89, %90, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %92 = stablehlo.slice %81 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %93 = stablehlo.convert %92 : (tensor<3xi32>) -> tensor<3xf32>
        %94 = stablehlo.convert %83 : tensor<3xf32>
        %95 = stablehlo.dot_general %93, %94, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %96 = stablehlo.broadcast_in_dim %87, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %97 = stablehlo.broadcast_in_dim %91, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %98 = stablehlo.broadcast_in_dim %95, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %99 = stablehlo.concatenate %96, %97, %98, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %100 = stablehlo.slice %arg0 [5:6, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %101 = stablehlo.reshape %100 : (tensor<1x5xi32>) -> tensor<5xi32>
        %102 = stablehlo.slice %arg1 [5:6, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %103 = stablehlo.reshape %102 : (tensor<1x3xf32>) -> tensor<3xf32>
        %104 = stablehlo.slice %101 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %105 = stablehlo.convert %104 : (tensor<3xi32>) -> tensor<3xf32>
        %106 = stablehlo.convert %103 : tensor<3xf32>
        %107 = stablehlo.dot_general %105, %106, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %108 = stablehlo.slice %101 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %109 = stablehlo.convert %108 : (tensor<3xi32>) -> tensor<3xf32>
        %110 = stablehlo.convert %103 : tensor<3xf32>
        %111 = stablehlo.dot_general %109, %110, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %112 = stablehlo.slice %101 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %113 = stablehlo.convert %112 : (tensor<3xi32>) -> tensor<3xf32>
        %114 = stablehlo.convert %103 : tensor<3xf32>
        %115 = stablehlo.dot_general %113, %114, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %116 = stablehlo.broadcast_in_dim %107, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %117 = stablehlo.broadcast_in_dim %111, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %118 = stablehlo.broadcast_in_dim %115, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %119 = stablehlo.concatenate %116, %117, %118, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %120 = stablehlo.slice %arg0 [6:7, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %121 = stablehlo.reshape %120 : (tensor<1x5xi32>) -> tensor<5xi32>
        %122 = stablehlo.slice %arg1 [6:7, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %123 = stablehlo.reshape %122 : (tensor<1x3xf32>) -> tensor<3xf32>
        %124 = stablehlo.slice %121 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %125 = stablehlo.convert %124 : (tensor<3xi32>) -> tensor<3xf32>
        %126 = stablehlo.convert %123 : tensor<3xf32>
        %127 = stablehlo.dot_general %125, %126, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %128 = stablehlo.slice %121 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %129 = stablehlo.convert %128 : (tensor<3xi32>) -> tensor<3xf32>
        %130 = stablehlo.convert %123 : tensor<3xf32>
        %131 = stablehlo.dot_general %129, %130, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %132 = stablehlo.slice %121 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %133 = stablehlo.convert %132 : (tensor<3xi32>) -> tensor<3xf32>
        %134 = stablehlo.convert %123 : tensor<3xf32>
        %135 = stablehlo.dot_general %133, %134, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %136 = stablehlo.broadcast_in_dim %127, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %137 = stablehlo.broadcast_in_dim %131, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %138 = stablehlo.broadcast_in_dim %135, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %139 = stablehlo.concatenate %136, %137, %138, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %140 = stablehlo.slice %arg0 [7:8, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %141 = stablehlo.reshape %140 : (tensor<1x5xi32>) -> tensor<5xi32>
        %142 = stablehlo.slice %arg1 [7:8, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %143 = stablehlo.reshape %142 : (tensor<1x3xf32>) -> tensor<3xf32>
        %144 = stablehlo.slice %141 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %145 = stablehlo.convert %144 : (tensor<3xi32>) -> tensor<3xf32>
        %146 = stablehlo.convert %143 : tensor<3xf32>
        %147 = stablehlo.dot_general %145, %146, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %148 = stablehlo.slice %141 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %149 = stablehlo.convert %148 : (tensor<3xi32>) -> tensor<3xf32>
        %150 = stablehlo.convert %143 : tensor<3xf32>
        %151 = stablehlo.dot_general %149, %150, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %152 = stablehlo.slice %141 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %153 = stablehlo.convert %152 : (tensor<3xi32>) -> tensor<3xf32>
        %154 = stablehlo.convert %143 : tensor<3xf32>
        %155 = stablehlo.dot_general %153, %154, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %156 = stablehlo.broadcast_in_dim %147, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %157 = stablehlo.broadcast_in_dim %151, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %158 = stablehlo.broadcast_in_dim %155, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %159 = stablehlo.concatenate %156, %157, %158, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %160 = stablehlo.slice %arg0 [8:9, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %161 = stablehlo.reshape %160 : (tensor<1x5xi32>) -> tensor<5xi32>
        %162 = stablehlo.slice %arg1 [8:9, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %163 = stablehlo.reshape %162 : (tensor<1x3xf32>) -> tensor<3xf32>
        %164 = stablehlo.slice %161 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %165 = stablehlo.convert %164 : (tensor<3xi32>) -> tensor<3xf32>
        %166 = stablehlo.convert %163 : tensor<3xf32>
        %167 = stablehlo.dot_general %165, %166, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %168 = stablehlo.slice %161 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %169 = stablehlo.convert %168 : (tensor<3xi32>) -> tensor<3xf32>
        %170 = stablehlo.convert %163 : tensor<3xf32>
        %171 = stablehlo.dot_general %169, %170, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %172 = stablehlo.slice %161 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %173 = stablehlo.convert %172 : (tensor<3xi32>) -> tensor<3xf32>
        %174 = stablehlo.convert %163 : tensor<3xf32>
        %175 = stablehlo.dot_general %173, %174, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %176 = stablehlo.broadcast_in_dim %167, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %177 = stablehlo.broadcast_in_dim %171, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %178 = stablehlo.broadcast_in_dim %175, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %179 = stablehlo.concatenate %176, %177, %178, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %180 = stablehlo.slice %arg0 [9:10, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %181 = stablehlo.reshape %180 : (tensor<1x5xi32>) -> tensor<5xi32>
        %182 = stablehlo.slice %arg1 [9:10, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %183 = stablehlo.reshape %182 : (tensor<1x3xf32>) -> tensor<3xf32>
        %184 = stablehlo.slice %181 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %185 = stablehlo.convert %184 : (tensor<3xi32>) -> tensor<3xf32>
        %186 = stablehlo.convert %183 : tensor<3xf32>
        %187 = stablehlo.dot_general %185, %186, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %188 = stablehlo.slice %181 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %189 = stablehlo.convert %188 : (tensor<3xi32>) -> tensor<3xf32>
        %190 = stablehlo.convert %183 : tensor<3xf32>
        %191 = stablehlo.dot_general %189, %190, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %192 = stablehlo.slice %181 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %193 = stablehlo.convert %192 : (tensor<3xi32>) -> tensor<3xf32>
        %194 = stablehlo.convert %183 : tensor<3xf32>
        %195 = stablehlo.dot_general %193, %194, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %196 = stablehlo.broadcast_in_dim %187, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %197 = stablehlo.broadcast_in_dim %191, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %198 = stablehlo.broadcast_in_dim %195, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %199 = stablehlo.concatenate %196, %197, %198, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %200 = stablehlo.slice %arg0 [10:11, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %201 = stablehlo.reshape %200 : (tensor<1x5xi32>) -> tensor<5xi32>
        %202 = stablehlo.slice %arg1 [10:11, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %203 = stablehlo.reshape %202 : (tensor<1x3xf32>) -> tensor<3xf32>
        %204 = stablehlo.slice %201 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %205 = stablehlo.convert %204 : (tensor<3xi32>) -> tensor<3xf32>
        %206 = stablehlo.convert %203 : tensor<3xf32>
        %207 = stablehlo.dot_general %205, %206, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %208 = stablehlo.slice %201 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %209 = stablehlo.convert %208 : (tensor<3xi32>) -> tensor<3xf32>
        %210 = stablehlo.convert %203 : tensor<3xf32>
        %211 = stablehlo.dot_general %209, %210, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %212 = stablehlo.slice %201 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %213 = stablehlo.convert %212 : (tensor<3xi32>) -> tensor<3xf32>
        %214 = stablehlo.convert %203 : tensor<3xf32>
        %215 = stablehlo.dot_general %213, %214, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %216 = stablehlo.broadcast_in_dim %207, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %217 = stablehlo.broadcast_in_dim %211, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %218 = stablehlo.broadcast_in_dim %215, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %219 = stablehlo.concatenate %216, %217, %218, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %220 = stablehlo.slice %arg0 [11:12, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %221 = stablehlo.reshape %220 : (tensor<1x5xi32>) -> tensor<5xi32>
        %222 = stablehlo.slice %arg1 [11:12, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %223 = stablehlo.reshape %222 : (tensor<1x3xf32>) -> tensor<3xf32>
        %224 = stablehlo.slice %221 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %225 = stablehlo.convert %224 : (tensor<3xi32>) -> tensor<3xf32>
        %226 = stablehlo.convert %223 : tensor<3xf32>
        %227 = stablehlo.dot_general %225, %226, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %228 = stablehlo.slice %221 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %229 = stablehlo.convert %228 : (tensor<3xi32>) -> tensor<3xf32>
        %230 = stablehlo.convert %223 : tensor<3xf32>
        %231 = stablehlo.dot_general %229, %230, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %232 = stablehlo.slice %221 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %233 = stablehlo.convert %232 : (tensor<3xi32>) -> tensor<3xf32>
        %234 = stablehlo.convert %223 : tensor<3xf32>
        %235 = stablehlo.dot_general %233, %234, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %236 = stablehlo.broadcast_in_dim %227, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %237 = stablehlo.broadcast_in_dim %231, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %238 = stablehlo.broadcast_in_dim %235, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %239 = stablehlo.concatenate %236, %237, %238, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %240 = stablehlo.slice %arg0 [12:13, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %241 = stablehlo.reshape %240 : (tensor<1x5xi32>) -> tensor<5xi32>
        %242 = stablehlo.slice %arg1 [12:13, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %243 = stablehlo.reshape %242 : (tensor<1x3xf32>) -> tensor<3xf32>
        %244 = stablehlo.slice %241 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %245 = stablehlo.convert %244 : (tensor<3xi32>) -> tensor<3xf32>
        %246 = stablehlo.convert %243 : tensor<3xf32>
        %247 = stablehlo.dot_general %245, %246, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %248 = stablehlo.slice %241 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %249 = stablehlo.convert %248 : (tensor<3xi32>) -> tensor<3xf32>
        %250 = stablehlo.convert %243 : tensor<3xf32>
        %251 = stablehlo.dot_general %249, %250, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %252 = stablehlo.slice %241 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %253 = stablehlo.convert %252 : (tensor<3xi32>) -> tensor<3xf32>
        %254 = stablehlo.convert %243 : tensor<3xf32>
        %255 = stablehlo.dot_general %253, %254, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %256 = stablehlo.broadcast_in_dim %247, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %257 = stablehlo.broadcast_in_dim %251, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %258 = stablehlo.broadcast_in_dim %255, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %259 = stablehlo.concatenate %256, %257, %258, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %260 = stablehlo.slice %arg0 [13:14, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %261 = stablehlo.reshape %260 : (tensor<1x5xi32>) -> tensor<5xi32>
        %262 = stablehlo.slice %arg1 [13:14, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %263 = stablehlo.reshape %262 : (tensor<1x3xf32>) -> tensor<3xf32>
        %264 = stablehlo.slice %261 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %265 = stablehlo.convert %264 : (tensor<3xi32>) -> tensor<3xf32>
        %266 = stablehlo.convert %263 : tensor<3xf32>
        %267 = stablehlo.dot_general %265, %266, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %268 = stablehlo.slice %261 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %269 = stablehlo.convert %268 : (tensor<3xi32>) -> tensor<3xf32>
        %270 = stablehlo.convert %263 : tensor<3xf32>
        %271 = stablehlo.dot_general %269, %270, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %272 = stablehlo.slice %261 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %273 = stablehlo.convert %272 : (tensor<3xi32>) -> tensor<3xf32>
        %274 = stablehlo.convert %263 : tensor<3xf32>
        %275 = stablehlo.dot_general %273, %274, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %276 = stablehlo.broadcast_in_dim %267, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %277 = stablehlo.broadcast_in_dim %271, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %278 = stablehlo.broadcast_in_dim %275, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %279 = stablehlo.concatenate %276, %277, %278, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %280 = stablehlo.slice %arg0 [14:15, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %281 = stablehlo.reshape %280 : (tensor<1x5xi32>) -> tensor<5xi32>
        %282 = stablehlo.slice %arg1 [14:15, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %283 = stablehlo.reshape %282 : (tensor<1x3xf32>) -> tensor<3xf32>
        %284 = stablehlo.slice %281 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %285 = stablehlo.convert %284 : (tensor<3xi32>) -> tensor<3xf32>
        %286 = stablehlo.convert %283 : tensor<3xf32>
        %287 = stablehlo.dot_general %285, %286, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %288 = stablehlo.slice %281 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %289 = stablehlo.convert %288 : (tensor<3xi32>) -> tensor<3xf32>
        %290 = stablehlo.convert %283 : tensor<3xf32>
        %291 = stablehlo.dot_general %289, %290, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %292 = stablehlo.slice %281 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %293 = stablehlo.convert %292 : (tensor<3xi32>) -> tensor<3xf32>
        %294 = stablehlo.convert %283 : tensor<3xf32>
        %295 = stablehlo.dot_general %293, %294, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %296 = stablehlo.broadcast_in_dim %287, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %297 = stablehlo.broadcast_in_dim %291, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %298 = stablehlo.broadcast_in_dim %295, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %299 = stablehlo.concatenate %296, %297, %298, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %300 = stablehlo.slice %arg0 [15:16, 0:5] : (tensor<16x5xi32>) -> tensor<1x5xi32>
        %301 = stablehlo.reshape %300 : (tensor<1x5xi32>) -> tensor<5xi32>
        %302 = stablehlo.slice %arg1 [15:16, 0:3] : (tensor<16x3xf32>) -> tensor<1x3xf32>
        %303 = stablehlo.reshape %302 : (tensor<1x3xf32>) -> tensor<3xf32>
        %304 = stablehlo.slice %301 [0:3] : (tensor<5xi32>) -> tensor<3xi32>
        %305 = stablehlo.convert %304 : (tensor<3xi32>) -> tensor<3xf32>
        %306 = stablehlo.convert %303 : tensor<3xf32>
        %307 = stablehlo.dot_general %305, %306, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %308 = stablehlo.slice %301 [1:4] : (tensor<5xi32>) -> tensor<3xi32>
        %309 = stablehlo.convert %308 : (tensor<3xi32>) -> tensor<3xf32>
        %310 = stablehlo.convert %303 : tensor<3xf32>
        %311 = stablehlo.dot_general %309, %310, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %312 = stablehlo.slice %301 [2:5] : (tensor<5xi32>) -> tensor<3xi32>
        %313 = stablehlo.convert %312 : (tensor<3xi32>) -> tensor<3xf32>
        %314 = stablehlo.convert %303 : tensor<3xf32>
        %315 = stablehlo.dot_general %313, %314, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<3xf32>, tensor<3xf32>) -> tensor<f32>
        %316 = stablehlo.broadcast_in_dim %307, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %317 = stablehlo.broadcast_in_dim %311, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %318 = stablehlo.broadcast_in_dim %315, dims = [] : (tensor<f32>) -> tensor<1xf32>
        %319 = stablehlo.concatenate %316, %317, %318, dim = 0 : (tensor<1xf32>, tensor<1xf32>, tensor<1xf32>) -> tensor<3xf32>
        %320 = stablehlo.broadcast_in_dim %19, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %321 = stablehlo.broadcast_in_dim %39, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %322 = stablehlo.broadcast_in_dim %59, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %323 = stablehlo.broadcast_in_dim %79, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %324 = stablehlo.broadcast_in_dim %99, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %325 = stablehlo.broadcast_in_dim %119, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %326 = stablehlo.broadcast_in_dim %139, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %327 = stablehlo.broadcast_in_dim %159, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %328 = stablehlo.broadcast_in_dim %179, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %329 = stablehlo.broadcast_in_dim %199, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %330 = stablehlo.broadcast_in_dim %219, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %331 = stablehlo.broadcast_in_dim %239, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %332 = stablehlo.broadcast_in_dim %259, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %333 = stablehlo.broadcast_in_dim %279, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %334 = stablehlo.broadcast_in_dim %299, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %335 = stablehlo.broadcast_in_dim %319, dims = [1] : (tensor<3xf32>) -> tensor<1x3xf32>
        %336 = stablehlo.concatenate %320, %321, %322, %323, %324, %325, %326, %327, %328, %329, %330, %331, %332, %333, %334, %335, dim = 0 : (tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>, tensor<1x3xf32>) -> tensor<16x3xf32>
        return %336 : tensor<16x3xf32>
      }
    }
    



```python

```


```python

```


```python

```


```python
train_images = np.array(mnist_dataset.data).reshape(len(mnist_dataset.data), -1)
train_labels = one_hot(np.array(mnist_dataset.targets), n_targets)
```


```python
mnist_dataset_test = MNIST('/tmp/mnist/', download=True, train=False)
test_images = jnp.array(mnist_dataset_test.data.numpy().reshape(len(mnist_dataset_test.data), -1), dtype=jnp.float32)
test_labels = one_hot(np.array(mnist_dataset_test.targets), n_targets)
```


```python
import time

first_time = True
print(num_epochs)
for epoch in range(num_epochs):
    start_time = time.time()
    for x, y in training_generator:
        y = one_hot(y, n_targets)
        # compiled_ = None
        if first_time:
            first_time = False
            saved_ = jit(update, device=jax.device_get(0)).lower(params, x, y)
            # # Print the lowered IR as text
            print(saved_.as_text())
            
            # Get the StableHLO representation of the lowered IR 
            print(saved_.compiler_ir(dialect="stablehlo"))
            compiled_ = saved_.compile()
        
        # compiled_(params, x, y)
        update(params, x, y)
    epoch_time = time.time() - start_time

    train_acc = accuracy(params, train_images, train_labels)
    test_acc = accuracy(params, test_images, test_labels)
    print("Epoch {} in {:0.2f} sec".format(epoch, epoch_time))
    print("Training set accuracy {}".format(train_acc))
    print("Test set accuracy {}".format(test_acc))
```

    10
    module @jit_update attributes {mhlo.num_partitions = 1 : i32, mhlo.num_replicas = 1 : i32} {
      func.func public @main(%arg0: tensor<512x784xf32> {mhlo.layout_mode = "default"}, %arg1: tensor<512xf32> {mhlo.layout_mode = "default"}, %arg2: tensor<512x512xf32> {mhlo.layout_mode = "default"}, %arg3: tensor<512xf32> {mhlo.layout_mode = "default"}, %arg4: tensor<10x512xf32> {mhlo.layout_mode = "default"}, %arg5: tensor<10xf32> {mhlo.layout_mode = "default"}, %arg6: tensor<128x784xf32> {mhlo.layout_mode = "default"}, %arg7: tensor<128x10xf32> {mhlo.layout_mode = "default"}) -> (tensor<512x784xf32> {jax.result_info = "[0][0]", mhlo.layout_mode = "default"}, tensor<512xf32> {jax.result_info = "[0][1]", mhlo.layout_mode = "default"}, tensor<512x512xf32> {jax.result_info = "[1][0]", mhlo.layout_mode = "default"}, tensor<512xf32> {jax.result_info = "[1][1]", mhlo.layout_mode = "default"}, tensor<10x512xf32> {jax.result_info = "[2][0]", mhlo.layout_mode = "default"}, tensor<10xf32> {jax.result_info = "[2][1]", mhlo.layout_mode = "default"}) {
        %0:6 = call @update(%arg0, %arg1, %arg2, %arg3, %arg4, %arg5, %arg6, %arg7) : (tensor<512x784xf32>, tensor<512xf32>, tensor<512x512xf32>, tensor<512xf32>, tensor<10x512xf32>, tensor<10xf32>, tensor<128x784xf32>, tensor<128x10xf32>) -> (tensor<512x784xf32>, tensor<512xf32>, tensor<512x512xf32>, tensor<512xf32>, tensor<10x512xf32>, tensor<10xf32>)
        return %0#0, %0#1, %0#2, %0#3, %0#4, %0#5 : tensor<512x784xf32>, tensor<512xf32>, tensor<512x512xf32>, tensor<512xf32>, tensor<10x512xf32>, tensor<10xf32>
      }
      func.func private @update(%arg0: tensor<512x784xf32> {mhlo.layout_mode = "default"}, %arg1: tensor<512xf32> {mhlo.layout_mode = "default"}, %arg2: tensor<512x512xf32> {mhlo.layout_mode = "default"}, %arg3: tensor<512xf32> {mhlo.layout_mode = "default"}, %arg4: tensor<10x512xf32> {mhlo.layout_mode = "default"}, %arg5: tensor<10xf32> {mhlo.layout_mode = "default"}, %arg6: tensor<128x784xf32> {mhlo.layout_mode = "default"}, %arg7: tensor<128x10xf32> {mhlo.layout_mode = "default"}) -> (tensor<512x784xf32> {mhlo.layout_mode = "default"}, tensor<512xf32> {mhlo.layout_mode = "default"}, tensor<512x512xf32> {mhlo.layout_mode = "default"}, tensor<512xf32> {mhlo.layout_mode = "default"}, tensor<10x512xf32> {mhlo.layout_mode = "default"}, tensor<10xf32> {mhlo.layout_mode = "default"}) {
        %0 = stablehlo.dot_general %arg0, %arg6, contracting_dims = [1] x [1], precision = [DEFAULT, DEFAULT] : (tensor<512x784xf32>, tensor<128x784xf32>) -> tensor<512x128xf32>
        %1 = stablehlo.transpose %0, dims = [1, 0] : (tensor<512x128xf32>) -> tensor<128x512xf32>
        %2 = stablehlo.broadcast_in_dim %arg1, dims = [1] : (tensor<512xf32>) -> tensor<1x512xf32>
        %3 = stablehlo.broadcast_in_dim %2, dims = [0, 1] : (tensor<1x512xf32>) -> tensor<128x512xf32>
        %4 = stablehlo.add %1, %3 : tensor<128x512xf32>
        %cst = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %5 = stablehlo.broadcast_in_dim %cst, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %6 = stablehlo.maximum %5, %4 : tensor<128x512xf32>
        %7 = stablehlo.compare  EQ, %4, %6,  FLOAT : (tensor<128x512xf32>, tensor<128x512xf32>) -> tensor<128x512xi1>
        %cst_0 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %8 = stablehlo.broadcast_in_dim %cst_0, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %cst_1 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %9 = stablehlo.broadcast_in_dim %cst_1, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %10 = stablehlo.select %7, %8, %9 : tensor<128x512xi1>, tensor<128x512xf32>
        %cst_2 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %11 = stablehlo.broadcast_in_dim %cst_2, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %12 = stablehlo.compare  EQ, %11, %6,  FLOAT : (tensor<128x512xf32>, tensor<128x512xf32>) -> tensor<128x512xi1>
        %cst_3 = stablehlo.constant dense<2.000000e+00> : tensor<f32>
        %13 = stablehlo.broadcast_in_dim %cst_3, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %cst_4 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %14 = stablehlo.broadcast_in_dim %cst_4, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %15 = stablehlo.select %12, %13, %14 : tensor<128x512xi1>, tensor<128x512xf32>
        %16 = stablehlo.divide %10, %15 : tensor<128x512xf32>
        %17 = stablehlo.dot_general %arg2, %6, contracting_dims = [1] x [1], precision = [DEFAULT, DEFAULT] : (tensor<512x512xf32>, tensor<128x512xf32>) -> tensor<512x128xf32>
        %18 = stablehlo.transpose %17, dims = [1, 0] : (tensor<512x128xf32>) -> tensor<128x512xf32>
        %19 = stablehlo.broadcast_in_dim %arg3, dims = [1] : (tensor<512xf32>) -> tensor<1x512xf32>
        %20 = stablehlo.broadcast_in_dim %19, dims = [0, 1] : (tensor<1x512xf32>) -> tensor<128x512xf32>
        %21 = stablehlo.add %18, %20 : tensor<128x512xf32>
        %cst_5 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %22 = stablehlo.broadcast_in_dim %cst_5, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %23 = stablehlo.maximum %22, %21 : tensor<128x512xf32>
        %24 = stablehlo.compare  EQ, %21, %23,  FLOAT : (tensor<128x512xf32>, tensor<128x512xf32>) -> tensor<128x512xi1>
        %cst_6 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %25 = stablehlo.broadcast_in_dim %cst_6, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %cst_7 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %26 = stablehlo.broadcast_in_dim %cst_7, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %27 = stablehlo.select %24, %25, %26 : tensor<128x512xi1>, tensor<128x512xf32>
        %cst_8 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %28 = stablehlo.broadcast_in_dim %cst_8, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %29 = stablehlo.compare  EQ, %28, %23,  FLOAT : (tensor<128x512xf32>, tensor<128x512xf32>) -> tensor<128x512xi1>
        %cst_9 = stablehlo.constant dense<2.000000e+00> : tensor<f32>
        %30 = stablehlo.broadcast_in_dim %cst_9, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %cst_10 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %31 = stablehlo.broadcast_in_dim %cst_10, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %32 = stablehlo.select %29, %30, %31 : tensor<128x512xi1>, tensor<128x512xf32>
        %33 = stablehlo.divide %27, %32 : tensor<128x512xf32>
        %34 = stablehlo.dot_general %arg4, %23, contracting_dims = [1] x [1], precision = [DEFAULT, DEFAULT] : (tensor<10x512xf32>, tensor<128x512xf32>) -> tensor<10x128xf32>
        %35 = stablehlo.transpose %34, dims = [1, 0] : (tensor<10x128xf32>) -> tensor<128x10xf32>
        %36 = stablehlo.broadcast_in_dim %arg5, dims = [1] : (tensor<10xf32>) -> tensor<1x10xf32>
        %37 = stablehlo.broadcast_in_dim %36, dims = [0, 1] : (tensor<1x10xf32>) -> tensor<128x10xf32>
        %38 = stablehlo.add %35, %37 : tensor<128x10xf32>
        %cst_11 = stablehlo.constant dense<0xFF800000> : tensor<f32>
        %39 = stablehlo.reduce(%38 init: %cst_11) applies stablehlo.maximum across dimensions = [1] : (tensor<128x10xf32>, tensor<f32>) -> tensor<128xf32>
        %cst_12 = stablehlo.constant dense<0xFF800000> : tensor<f32>
        %40 = stablehlo.broadcast_in_dim %cst_12, dims = [] : (tensor<f32>) -> tensor<128xf32>
        %41 = stablehlo.maximum %40, %39 : tensor<128xf32>
        %42 = stablehlo.is_finite %41 : (tensor<128xf32>) -> tensor<128xi1>
        %cst_13 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %43 = stablehlo.broadcast_in_dim %cst_13, dims = [] : (tensor<f32>) -> tensor<128xf32>
        %44 = stablehlo.select %42, %41, %43 : tensor<128xi1>, tensor<128xf32>
        %45 = stablehlo.broadcast_in_dim %44, dims = [0] : (tensor<128xf32>) -> tensor<128x1xf32>
        %46 = stablehlo.broadcast_in_dim %45, dims = [0, 1] : (tensor<128x1xf32>) -> tensor<128x10xf32>
        %47 = stablehlo.subtract %38, %46 : tensor<128x10xf32>
        %48 = stablehlo.exponential %47 : tensor<128x10xf32>
        %cst_14 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %49 = stablehlo.reduce(%48 init: %cst_14) applies stablehlo.add across dimensions = [1] : (tensor<128x10xf32>, tensor<f32>) -> tensor<128xf32>
        %50 = stablehlo.abs %49 : tensor<128xf32>
        %cst_15 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %51 = stablehlo.broadcast_in_dim %cst_15, dims = [] : (tensor<f32>) -> tensor<128xf32>
        %52 = stablehlo.compare  GE, %49, %51,  FLOAT : (tensor<128xf32>, tensor<128xf32>) -> tensor<128xi1>
        %cst_16 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %53 = stablehlo.negate %cst_16 : tensor<f32>
        %cst_17 = stablehlo.constant dense<1.280000e+03> : tensor<f32>
        %54 = stablehlo.divide %53, %cst_17 : tensor<f32>
        %55 = stablehlo.broadcast_in_dim %54, dims = [] : (tensor<f32>) -> tensor<128x10xf32>
        %56 = stablehlo.multiply %55, %arg7 : tensor<128x10xf32>
        %57 = stablehlo.negate %56 : tensor<128x10xf32>
        %cst_18 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %58 = stablehlo.reduce(%57 init: %cst_18) applies stablehlo.add across dimensions = [1] : (tensor<128x10xf32>, tensor<f32>) -> tensor<128xf32>
        %59 = stablehlo.reshape %58 : (tensor<128xf32>) -> tensor<128x1xf32>
        %cst_19 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %60 = stablehlo.reduce(%59 init: %cst_19) applies stablehlo.add across dimensions = [1] : (tensor<128x1xf32>, tensor<f32>) -> tensor<128xf32>
        %61 = stablehlo.divide %60, %50 : tensor<128xf32>
        %cst_20 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %62 = stablehlo.broadcast_in_dim %cst_20, dims = [] : (tensor<f32>) -> tensor<128xf32>
        %63 = stablehlo.select %52, %62, %61 : tensor<128xi1>, tensor<128xf32>
        %64 = stablehlo.select %52, %61, %62 : tensor<128xi1>, tensor<128xf32>
        %65 = stablehlo.negate %63 : tensor<128xf32>
        %66 = stablehlo.add %64, %65 : tensor<128xf32>
        %67 = stablehlo.broadcast_in_dim %66, dims = [0] : (tensor<128xf32>) -> tensor<128x10xf32>
        %68 = stablehlo.multiply %67, %48 : tensor<128x10xf32>
        %69 = stablehlo.add %56, %68 : tensor<128x10xf32>
        %cst_21 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %70 = stablehlo.reduce(%69 init: %cst_21) applies stablehlo.add across dimensions = [0] : (tensor<128x10xf32>, tensor<f32>) -> tensor<10xf32>
        %71 = stablehlo.reshape %70 : (tensor<10xf32>) -> tensor<1x10xf32>
        %cst_22 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %72 = stablehlo.reduce(%71 init: %cst_22) applies stablehlo.add across dimensions = [0] : (tensor<1x10xf32>, tensor<f32>) -> tensor<10xf32>
        %73 = stablehlo.transpose %69, dims = [1, 0] : (tensor<128x10xf32>) -> tensor<10x128xf32>
        %74 = stablehlo.dot_general %73, %arg4, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<10x128xf32>, tensor<10x512xf32>) -> tensor<128x512xf32>
        %75 = stablehlo.dot_general %73, %23, contracting_dims = [1] x [0], precision = [DEFAULT, DEFAULT] : (tensor<10x128xf32>, tensor<128x512xf32>) -> tensor<10x512xf32>
        %76 = stablehlo.multiply %74, %33 : tensor<128x512xf32>
        %cst_23 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %77 = stablehlo.reduce(%76 init: %cst_23) applies stablehlo.add across dimensions = [0] : (tensor<128x512xf32>, tensor<f32>) -> tensor<512xf32>
        %78 = stablehlo.reshape %77 : (tensor<512xf32>) -> tensor<1x512xf32>
        %cst_24 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %79 = stablehlo.reduce(%78 init: %cst_24) applies stablehlo.add across dimensions = [0] : (tensor<1x512xf32>, tensor<f32>) -> tensor<512xf32>
        %80 = stablehlo.transpose %76, dims = [1, 0] : (tensor<128x512xf32>) -> tensor<512x128xf32>
        %81 = stablehlo.dot_general %80, %arg2, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<512x128xf32>, tensor<512x512xf32>) -> tensor<128x512xf32>
        %82 = stablehlo.dot_general %80, %6, contracting_dims = [1] x [0], precision = [DEFAULT, DEFAULT] : (tensor<512x128xf32>, tensor<128x512xf32>) -> tensor<512x512xf32>
        %83 = stablehlo.multiply %81, %16 : tensor<128x512xf32>
        %cst_25 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %84 = stablehlo.reduce(%83 init: %cst_25) applies stablehlo.add across dimensions = [0] : (tensor<128x512xf32>, tensor<f32>) -> tensor<512xf32>
        %85 = stablehlo.reshape %84 : (tensor<512xf32>) -> tensor<1x512xf32>
        %cst_26 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %86 = stablehlo.reduce(%85 init: %cst_26) applies stablehlo.add across dimensions = [0] : (tensor<1x512xf32>, tensor<f32>) -> tensor<512xf32>
        %87 = stablehlo.transpose %83, dims = [1, 0] : (tensor<128x512xf32>) -> tensor<512x128xf32>
        %88 = stablehlo.dot_general %87, %arg6, contracting_dims = [1] x [0], precision = [DEFAULT, DEFAULT] : (tensor<512x128xf32>, tensor<128x784xf32>) -> tensor<512x784xf32>
        %cst_27 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %89 = stablehlo.broadcast_in_dim %cst_27, dims = [] : (tensor<f32>) -> tensor<512x784xf32>
        %90 = stablehlo.multiply %89, %88 : tensor<512x784xf32>
        %91 = stablehlo.subtract %arg0, %90 : tensor<512x784xf32>
        %cst_28 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %92 = stablehlo.broadcast_in_dim %cst_28, dims = [] : (tensor<f32>) -> tensor<512xf32>
        %93 = stablehlo.multiply %92, %86 : tensor<512xf32>
        %94 = stablehlo.subtract %arg1, %93 : tensor<512xf32>
        %cst_29 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %95 = stablehlo.broadcast_in_dim %cst_29, dims = [] : (tensor<f32>) -> tensor<512x512xf32>
        %96 = stablehlo.multiply %95, %82 : tensor<512x512xf32>
        %97 = stablehlo.subtract %arg2, %96 : tensor<512x512xf32>
        %cst_30 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %98 = stablehlo.broadcast_in_dim %cst_30, dims = [] : (tensor<f32>) -> tensor<512xf32>
        %99 = stablehlo.multiply %98, %79 : tensor<512xf32>
        %100 = stablehlo.subtract %arg3, %99 : tensor<512xf32>
        %cst_31 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %101 = stablehlo.broadcast_in_dim %cst_31, dims = [] : (tensor<f32>) -> tensor<10x512xf32>
        %102 = stablehlo.multiply %101, %75 : tensor<10x512xf32>
        %103 = stablehlo.subtract %arg4, %102 : tensor<10x512xf32>
        %cst_32 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %104 = stablehlo.broadcast_in_dim %cst_32, dims = [] : (tensor<f32>) -> tensor<10xf32>
        %105 = stablehlo.multiply %104, %72 : tensor<10xf32>
        %106 = stablehlo.subtract %arg5, %105 : tensor<10xf32>
        return %91, %94, %97, %100, %103, %106 : tensor<512x784xf32>, tensor<512xf32>, tensor<512x512xf32>, tensor<512xf32>, tensor<10x512xf32>, tensor<10xf32>
      }
    }
    
    module @jit_update attributes {mhlo.num_partitions = 1 : i32, mhlo.num_replicas = 1 : i32} {
      func.func public @main(%arg0: tensor<512x784xf32> {mhlo.layout_mode = "default"}, %arg1: tensor<512xf32> {mhlo.layout_mode = "default"}, %arg2: tensor<512x512xf32> {mhlo.layout_mode = "default"}, %arg3: tensor<512xf32> {mhlo.layout_mode = "default"}, %arg4: tensor<10x512xf32> {mhlo.layout_mode = "default"}, %arg5: tensor<10xf32> {mhlo.layout_mode = "default"}, %arg6: tensor<128x784xf32> {mhlo.layout_mode = "default"}, %arg7: tensor<128x10xf32> {mhlo.layout_mode = "default"}) -> (tensor<512x784xf32> {jax.result_info = "[0][0]", mhlo.layout_mode = "default"}, tensor<512xf32> {jax.result_info = "[0][1]", mhlo.layout_mode = "default"}, tensor<512x512xf32> {jax.result_info = "[1][0]", mhlo.layout_mode = "default"}, tensor<512xf32> {jax.result_info = "[1][1]", mhlo.layout_mode = "default"}, tensor<10x512xf32> {jax.result_info = "[2][0]", mhlo.layout_mode = "default"}, tensor<10xf32> {jax.result_info = "[2][1]", mhlo.layout_mode = "default"}) {
        %0:6 = call @update(%arg0, %arg1, %arg2, %arg3, %arg4, %arg5, %arg6, %arg7) : (tensor<512x784xf32>, tensor<512xf32>, tensor<512x512xf32>, tensor<512xf32>, tensor<10x512xf32>, tensor<10xf32>, tensor<128x784xf32>, tensor<128x10xf32>) -> (tensor<512x784xf32>, tensor<512xf32>, tensor<512x512xf32>, tensor<512xf32>, tensor<10x512xf32>, tensor<10xf32>)
        return %0#0, %0#1, %0#2, %0#3, %0#4, %0#5 : tensor<512x784xf32>, tensor<512xf32>, tensor<512x512xf32>, tensor<512xf32>, tensor<10x512xf32>, tensor<10xf32>
      }
      func.func private @update(%arg0: tensor<512x784xf32> {mhlo.layout_mode = "default"}, %arg1: tensor<512xf32> {mhlo.layout_mode = "default"}, %arg2: tensor<512x512xf32> {mhlo.layout_mode = "default"}, %arg3: tensor<512xf32> {mhlo.layout_mode = "default"}, %arg4: tensor<10x512xf32> {mhlo.layout_mode = "default"}, %arg5: tensor<10xf32> {mhlo.layout_mode = "default"}, %arg6: tensor<128x784xf32> {mhlo.layout_mode = "default"}, %arg7: tensor<128x10xf32> {mhlo.layout_mode = "default"}) -> (tensor<512x784xf32> {mhlo.layout_mode = "default"}, tensor<512xf32> {mhlo.layout_mode = "default"}, tensor<512x512xf32> {mhlo.layout_mode = "default"}, tensor<512xf32> {mhlo.layout_mode = "default"}, tensor<10x512xf32> {mhlo.layout_mode = "default"}, tensor<10xf32> {mhlo.layout_mode = "default"}) {
        %0 = stablehlo.dot_general %arg0, %arg6, contracting_dims = [1] x [1], precision = [DEFAULT, DEFAULT] : (tensor<512x784xf32>, tensor<128x784xf32>) -> tensor<512x128xf32>
        %1 = stablehlo.transpose %0, dims = [1, 0] : (tensor<512x128xf32>) -> tensor<128x512xf32>
        %2 = stablehlo.broadcast_in_dim %arg1, dims = [1] : (tensor<512xf32>) -> tensor<1x512xf32>
        %3 = stablehlo.broadcast_in_dim %2, dims = [0, 1] : (tensor<1x512xf32>) -> tensor<128x512xf32>
        %4 = stablehlo.add %1, %3 : tensor<128x512xf32>
        %cst = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %5 = stablehlo.broadcast_in_dim %cst, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %6 = stablehlo.maximum %5, %4 : tensor<128x512xf32>
        %7 = stablehlo.compare  EQ, %4, %6,  FLOAT : (tensor<128x512xf32>, tensor<128x512xf32>) -> tensor<128x512xi1>
        %cst_0 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %8 = stablehlo.broadcast_in_dim %cst_0, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %cst_1 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %9 = stablehlo.broadcast_in_dim %cst_1, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %10 = stablehlo.select %7, %8, %9 : tensor<128x512xi1>, tensor<128x512xf32>
        %cst_2 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %11 = stablehlo.broadcast_in_dim %cst_2, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %12 = stablehlo.compare  EQ, %11, %6,  FLOAT : (tensor<128x512xf32>, tensor<128x512xf32>) -> tensor<128x512xi1>
        %cst_3 = stablehlo.constant dense<2.000000e+00> : tensor<f32>
        %13 = stablehlo.broadcast_in_dim %cst_3, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %cst_4 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %14 = stablehlo.broadcast_in_dim %cst_4, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %15 = stablehlo.select %12, %13, %14 : tensor<128x512xi1>, tensor<128x512xf32>
        %16 = stablehlo.divide %10, %15 : tensor<128x512xf32>
        %17 = stablehlo.dot_general %arg2, %6, contracting_dims = [1] x [1], precision = [DEFAULT, DEFAULT] : (tensor<512x512xf32>, tensor<128x512xf32>) -> tensor<512x128xf32>
        %18 = stablehlo.transpose %17, dims = [1, 0] : (tensor<512x128xf32>) -> tensor<128x512xf32>
        %19 = stablehlo.broadcast_in_dim %arg3, dims = [1] : (tensor<512xf32>) -> tensor<1x512xf32>
        %20 = stablehlo.broadcast_in_dim %19, dims = [0, 1] : (tensor<1x512xf32>) -> tensor<128x512xf32>
        %21 = stablehlo.add %18, %20 : tensor<128x512xf32>
        %cst_5 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %22 = stablehlo.broadcast_in_dim %cst_5, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %23 = stablehlo.maximum %22, %21 : tensor<128x512xf32>
        %24 = stablehlo.compare  EQ, %21, %23,  FLOAT : (tensor<128x512xf32>, tensor<128x512xf32>) -> tensor<128x512xi1>
        %cst_6 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %25 = stablehlo.broadcast_in_dim %cst_6, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %cst_7 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %26 = stablehlo.broadcast_in_dim %cst_7, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %27 = stablehlo.select %24, %25, %26 : tensor<128x512xi1>, tensor<128x512xf32>
        %cst_8 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %28 = stablehlo.broadcast_in_dim %cst_8, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %29 = stablehlo.compare  EQ, %28, %23,  FLOAT : (tensor<128x512xf32>, tensor<128x512xf32>) -> tensor<128x512xi1>
        %cst_9 = stablehlo.constant dense<2.000000e+00> : tensor<f32>
        %30 = stablehlo.broadcast_in_dim %cst_9, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %cst_10 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %31 = stablehlo.broadcast_in_dim %cst_10, dims = [] : (tensor<f32>) -> tensor<128x512xf32>
        %32 = stablehlo.select %29, %30, %31 : tensor<128x512xi1>, tensor<128x512xf32>
        %33 = stablehlo.divide %27, %32 : tensor<128x512xf32>
        %34 = stablehlo.dot_general %arg4, %23, contracting_dims = [1] x [1], precision = [DEFAULT, DEFAULT] : (tensor<10x512xf32>, tensor<128x512xf32>) -> tensor<10x128xf32>
        %35 = stablehlo.transpose %34, dims = [1, 0] : (tensor<10x128xf32>) -> tensor<128x10xf32>
        %36 = stablehlo.broadcast_in_dim %arg5, dims = [1] : (tensor<10xf32>) -> tensor<1x10xf32>
        %37 = stablehlo.broadcast_in_dim %36, dims = [0, 1] : (tensor<1x10xf32>) -> tensor<128x10xf32>
        %38 = stablehlo.add %35, %37 : tensor<128x10xf32>
        %cst_11 = stablehlo.constant dense<0xFF800000> : tensor<f32>
        %39 = stablehlo.reduce(%38 init: %cst_11) applies stablehlo.maximum across dimensions = [1] : (tensor<128x10xf32>, tensor<f32>) -> tensor<128xf32>
        %cst_12 = stablehlo.constant dense<0xFF800000> : tensor<f32>
        %40 = stablehlo.broadcast_in_dim %cst_12, dims = [] : (tensor<f32>) -> tensor<128xf32>
        %41 = stablehlo.maximum %40, %39 : tensor<128xf32>
        %42 = stablehlo.is_finite %41 : (tensor<128xf32>) -> tensor<128xi1>
        %cst_13 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %43 = stablehlo.broadcast_in_dim %cst_13, dims = [] : (tensor<f32>) -> tensor<128xf32>
        %44 = stablehlo.select %42, %41, %43 : tensor<128xi1>, tensor<128xf32>
        %45 = stablehlo.broadcast_in_dim %44, dims = [0] : (tensor<128xf32>) -> tensor<128x1xf32>
        %46 = stablehlo.broadcast_in_dim %45, dims = [0, 1] : (tensor<128x1xf32>) -> tensor<128x10xf32>
        %47 = stablehlo.subtract %38, %46 : tensor<128x10xf32>
        %48 = stablehlo.exponential %47 : tensor<128x10xf32>
        %cst_14 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %49 = stablehlo.reduce(%48 init: %cst_14) applies stablehlo.add across dimensions = [1] : (tensor<128x10xf32>, tensor<f32>) -> tensor<128xf32>
        %50 = stablehlo.abs %49 : tensor<128xf32>
        %cst_15 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %51 = stablehlo.broadcast_in_dim %cst_15, dims = [] : (tensor<f32>) -> tensor<128xf32>
        %52 = stablehlo.compare  GE, %49, %51,  FLOAT : (tensor<128xf32>, tensor<128xf32>) -> tensor<128xi1>
        %cst_16 = stablehlo.constant dense<1.000000e+00> : tensor<f32>
        %53 = stablehlo.negate %cst_16 : tensor<f32>
        %cst_17 = stablehlo.constant dense<1.280000e+03> : tensor<f32>
        %54 = stablehlo.divide %53, %cst_17 : tensor<f32>
        %55 = stablehlo.broadcast_in_dim %54, dims = [] : (tensor<f32>) -> tensor<128x10xf32>
        %56 = stablehlo.multiply %55, %arg7 : tensor<128x10xf32>
        %57 = stablehlo.negate %56 : tensor<128x10xf32>
        %cst_18 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %58 = stablehlo.reduce(%57 init: %cst_18) applies stablehlo.add across dimensions = [1] : (tensor<128x10xf32>, tensor<f32>) -> tensor<128xf32>
        %59 = stablehlo.reshape %58 : (tensor<128xf32>) -> tensor<128x1xf32>
        %cst_19 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %60 = stablehlo.reduce(%59 init: %cst_19) applies stablehlo.add across dimensions = [1] : (tensor<128x1xf32>, tensor<f32>) -> tensor<128xf32>
        %61 = stablehlo.divide %60, %50 : tensor<128xf32>
        %cst_20 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %62 = stablehlo.broadcast_in_dim %cst_20, dims = [] : (tensor<f32>) -> tensor<128xf32>
        %63 = stablehlo.select %52, %62, %61 : tensor<128xi1>, tensor<128xf32>
        %64 = stablehlo.select %52, %61, %62 : tensor<128xi1>, tensor<128xf32>
        %65 = stablehlo.negate %63 : tensor<128xf32>
        %66 = stablehlo.add %64, %65 : tensor<128xf32>
        %67 = stablehlo.broadcast_in_dim %66, dims = [0] : (tensor<128xf32>) -> tensor<128x10xf32>
        %68 = stablehlo.multiply %67, %48 : tensor<128x10xf32>
        %69 = stablehlo.add %56, %68 : tensor<128x10xf32>
        %cst_21 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %70 = stablehlo.reduce(%69 init: %cst_21) applies stablehlo.add across dimensions = [0] : (tensor<128x10xf32>, tensor<f32>) -> tensor<10xf32>
        %71 = stablehlo.reshape %70 : (tensor<10xf32>) -> tensor<1x10xf32>
        %cst_22 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %72 = stablehlo.reduce(%71 init: %cst_22) applies stablehlo.add across dimensions = [0] : (tensor<1x10xf32>, tensor<f32>) -> tensor<10xf32>
        %73 = stablehlo.transpose %69, dims = [1, 0] : (tensor<128x10xf32>) -> tensor<10x128xf32>
        %74 = stablehlo.dot_general %73, %arg4, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<10x128xf32>, tensor<10x512xf32>) -> tensor<128x512xf32>
        %75 = stablehlo.dot_general %73, %23, contracting_dims = [1] x [0], precision = [DEFAULT, DEFAULT] : (tensor<10x128xf32>, tensor<128x512xf32>) -> tensor<10x512xf32>
        %76 = stablehlo.multiply %74, %33 : tensor<128x512xf32>
        %cst_23 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %77 = stablehlo.reduce(%76 init: %cst_23) applies stablehlo.add across dimensions = [0] : (tensor<128x512xf32>, tensor<f32>) -> tensor<512xf32>
        %78 = stablehlo.reshape %77 : (tensor<512xf32>) -> tensor<1x512xf32>
        %cst_24 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %79 = stablehlo.reduce(%78 init: %cst_24) applies stablehlo.add across dimensions = [0] : (tensor<1x512xf32>, tensor<f32>) -> tensor<512xf32>
        %80 = stablehlo.transpose %76, dims = [1, 0] : (tensor<128x512xf32>) -> tensor<512x128xf32>
        %81 = stablehlo.dot_general %80, %arg2, contracting_dims = [0] x [0], precision = [DEFAULT, DEFAULT] : (tensor<512x128xf32>, tensor<512x512xf32>) -> tensor<128x512xf32>
        %82 = stablehlo.dot_general %80, %6, contracting_dims = [1] x [0], precision = [DEFAULT, DEFAULT] : (tensor<512x128xf32>, tensor<128x512xf32>) -> tensor<512x512xf32>
        %83 = stablehlo.multiply %81, %16 : tensor<128x512xf32>
        %cst_25 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %84 = stablehlo.reduce(%83 init: %cst_25) applies stablehlo.add across dimensions = [0] : (tensor<128x512xf32>, tensor<f32>) -> tensor<512xf32>
        %85 = stablehlo.reshape %84 : (tensor<512xf32>) -> tensor<1x512xf32>
        %cst_26 = stablehlo.constant dense<0.000000e+00> : tensor<f32>
        %86 = stablehlo.reduce(%85 init: %cst_26) applies stablehlo.add across dimensions = [0] : (tensor<1x512xf32>, tensor<f32>) -> tensor<512xf32>
        %87 = stablehlo.transpose %83, dims = [1, 0] : (tensor<128x512xf32>) -> tensor<512x128xf32>
        %88 = stablehlo.dot_general %87, %arg6, contracting_dims = [1] x [0], precision = [DEFAULT, DEFAULT] : (tensor<512x128xf32>, tensor<128x784xf32>) -> tensor<512x784xf32>
        %cst_27 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %89 = stablehlo.broadcast_in_dim %cst_27, dims = [] : (tensor<f32>) -> tensor<512x784xf32>
        %90 = stablehlo.multiply %89, %88 : tensor<512x784xf32>
        %91 = stablehlo.subtract %arg0, %90 : tensor<512x784xf32>
        %cst_28 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %92 = stablehlo.broadcast_in_dim %cst_28, dims = [] : (tensor<f32>) -> tensor<512xf32>
        %93 = stablehlo.multiply %92, %86 : tensor<512xf32>
        %94 = stablehlo.subtract %arg1, %93 : tensor<512xf32>
        %cst_29 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %95 = stablehlo.broadcast_in_dim %cst_29, dims = [] : (tensor<f32>) -> tensor<512x512xf32>
        %96 = stablehlo.multiply %95, %82 : tensor<512x512xf32>
        %97 = stablehlo.subtract %arg2, %96 : tensor<512x512xf32>
        %cst_30 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %98 = stablehlo.broadcast_in_dim %cst_30, dims = [] : (tensor<f32>) -> tensor<512xf32>
        %99 = stablehlo.multiply %98, %79 : tensor<512xf32>
        %100 = stablehlo.subtract %arg3, %99 : tensor<512xf32>
        %cst_31 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %101 = stablehlo.broadcast_in_dim %cst_31, dims = [] : (tensor<f32>) -> tensor<10x512xf32>
        %102 = stablehlo.multiply %101, %75 : tensor<10x512xf32>
        %103 = stablehlo.subtract %arg4, %102 : tensor<10x512xf32>
        %cst_32 = stablehlo.constant dense<0.00999999977> : tensor<f32>
        %104 = stablehlo.broadcast_in_dim %cst_32, dims = [] : (tensor<f32>) -> tensor<10xf32>
        %105 = stablehlo.multiply %104, %72 : tensor<10xf32>
        %106 = stablehlo.subtract %arg5, %105 : tensor<10xf32>
        return %91, %94, %97, %100, %103, %106 : tensor<512x784xf32>, tensor<512xf32>, tensor<512x512xf32>, tensor<512xf32>, tensor<10x512xf32>, tensor<10xf32>
      }
    }
    
    Epoch 0 in 11.98 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104
    Epoch 1 in 2.93 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104
    Epoch 2 in 2.82 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104
    Epoch 3 in 2.72 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104
    Epoch 4 in 2.73 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104
    Epoch 5 in 2.77 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104
    Epoch 6 in 2.80 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104
    Epoch 7 in 2.73 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104
    Epoch 8 in 2.74 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104
    Epoch 9 in 2.79 sec
    Training set accuracy 0.08895000070333481
    Test set accuracy 0.08959999680519104



```python

```


```python

```


```python
from jax import grad, jit, vmap

def my_predict(params, inputs):
    for W, b in params:
        outputs = jnp.dot(inputs, W) + b
        inputs = jnp.maximum(outputs, 0)
    return outputs

def loss(params, batch):
    inputs, targets = batch
    preds = my_predict(params, inputs)
    return jnp.sum((preds - targets) ** 2)

grad_func = jit(grad(loss))
per_example_grads = jit(vmap(grad(loss), in_axes=(None, 0)))
```


```python

```


```python

```


```python

```


```python

```


```python

```


```python

```


```python

```


```python

```


```python

```

