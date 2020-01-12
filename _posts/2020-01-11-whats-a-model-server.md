---
layout: post
title: "What is a Deep Learning Model Server?"
excerpt: "Deploy Your Deep Learning With Model Server"
tags: [Deep Learning, MXNet, TVM, AI, Compilers, Linux, OS, Model, Server, HTTP, MMS]
comments: true
---
A typical Deep Learning Workflow from a software engineering design perspective
is shown in the figure below. This is a very high level overview of how a
typical process flow works. There are exceptions to this when using Unsupervised
Learning Techniques, but we will ignore them for sake of simplicity.
![Typical Deep Learning Workflow](https://d2l.ai/_images/data-collection.svg)

In production systems, the most amount of time and compute resources are spent
in the iteration of ``Input -> Model -> Output`` cycle. It's the bottom row in
the figure. If you think about relatively mature models that perform
off-the-shelf techniques, ex: Object Detection, Image Classification, Text
Classification etc this is truer than other cases. Common Sense follows that it
is imperative that this control path be the focus of all optimizations. It's the
reason behind AWS launching a completely separate service called [SageMaker
Neo](https://aws.amazon.com/sagemaker/neo/). To be fair, optimal inference is
just one of the many aspects/features of this service. Check it out.

[TVM](https://tvm.apache.org/) is an open source project that aims to
standardize the representation of output from the ``Training`` phase of the
several Deep Learning Frameworks and optimize for the supported target hardwares
(plenty of them). ``TVM`` does this by optimizing the implementation of the
individual operators in the standardized representation for each supported
hardware architecture.

So far it's all sunshine and roses, correct? The trouble is when you smell
them. They stink! A harsh reality is that most of the efforts were (rightly)
spent into achieving a stage where requests can be sent continuously to make
inferences. But these inferences were simply put behind a [``Flask
App``](https://pythonspot.com/flask-web-app-with-python/) which is the
equivalent of having a single Raspberry Pi powered server to handle all the
search requests of Google (exaggerated but true).

So many big players have put out significant efforts to solve this issue using
their know-how. As a result, there are several acceptable solutions being used in
production by several players:
+ [TensorFlow Serving](https://github.com/tensorflow/serving)
+ [Clipper](https://github.com/ucbrise/clipper)
+ [Model Server for Apache MXNet](https://aws.amazon.com/blogs/ai/introducing-model-server-for-apache-mxnet/)
+ [DeepDetect](https://deepdetect.com/)
+ [TensorRT](https://developer.nvidia.com/tensorrt)

To give you a bird's view here's the MXNet Solution from the [AWS Blog](https://aws.amazon.com/blogs/ai/introducing-model-server-for-apache-mxnet/)
![MXNet Model Server](https://d2908q01vomqb2.cloudfront.net/f1f836cb4ea6efb2a0b1b99f41ad8b103eff4b59/2017/12/07/model-server-1.gif)

### Start the Server:
Looking at the documentation, it is pretty easy to get started. 
```bash
mxnet-model-server --models squeezenet=https://s3.amazonaws.com/model-server/models/squeezenet_v1.1/squeezenet_v1.1.model
```

### Request from Client:
Now the client can send HTTP requests to obtain inferences
```bash
curl -O https://s3.amazonaws.com/model-server/inputs/kitten.jpg
curl http://127.0.0.1:8080/squeezenet/predict -F "input0=@kitten.jpg"
```

### Get a Result:
```bash
{
  "prediction": [
    [
      {
        "class": "n02124075 Egyptian cat",
        "probability": 0.8515276312828064
      },
… (other lower probability predictions) …
}
```

The reason I chose Apache's Solution here is because after my ~~cursory
browsing~~ research that seemed like the most mature option with decent amount
of features.

### Model Archive:
It offers a feature to archive the model into a common format/representation
which allows it to support Models from various Deep Learning Frameworks.
https://d2908q01vomqb2.cloudfront.net/f1f836cb4ea6efb2a0b1b99f41ad8b103eff4b59/2017/12/07/model-server-2.gif

#### Source:
+ [Article from a couple of years ago](https://medium.com/@vikati/the-rise-of-the-model-servers-9395522b6c58)
+ [TensorFlow Serving](https://github.com/tensorflow/serving)
+ [Clipper](https://github.com/ucbrise/clipper)
+ [Model Server for Apache MXNet](https://aws.amazon.com/blogs/ai/introducing-model-server-for-apache-mxnet/)
+ [DeepDetect](https://deepdetect.com/)
+ [TensorRT](https://developer.nvidia.com/tensorrt)
+ [Model Serving in PyTorch](https://pytorch.org/blog/model-serving-in-pyorch/)
+ [AutoGluon](https://www.amazon.science/amazons-autogluon-helps-developers-get-up-and-running-with-state-of-the-art-deep-learning-models-with-just-a-few-lines-of-code)
+ [TVM](https://tvm.apache.org/)
