---
layout: post
title: "PySpark: Running on EMR Clusters with SageMaker Notebooks"
excerpt: "PySpark on SageMaker"
tags: [Cluster, Computing, EMR, Spark, ML, PySpark]
comments: true
---

## What is PySpark
Apache Spark is one of the most popular distributed computation framework
available. Most popular for the ability it provides to perform seamless data
analysis. It is also fast becoming the choice for performing Machine Learning
tasks. It provides native support for Scala APIs.
PySpark is the package that provides Python API interface to PySpark.

## What is EMR
The central component of Amazon EMR is the cluster. A cluster is a collection
of Amazon Elastic Compute Cloud (Amazon EC2) instances. Each instance in the
cluster is called a node. Each node has a role within the cluster, referred to
as the node type. Amazon EMR also installs different software components on each
node type, giving each node a role in a distributed application like Apache
Hadoop.

## Why SageMaker Notebook
Amazon SageMaker provides a fully managed service for data science and machine
learning workflows. One of the important parts of Amazon SageMaker is the
powerful Jupyter notebook interface, which can be used to build models.

## How to Set this up in no time
https://aws.amazon.com/blogs/machine-learning/build-amazon-sagemaker-notebooks-backed-by-spark-in-amazon-emr/

## Why would use these combo technologies
I recently had a request to crunch through a huge dataset that had some strange
requirements (not so strange actually). However, this got me smacking my lips to
learn about Spark and actually have a project to show for it. I decided to use
the bleeding edge.

The initial set of questins had me asking - how should I allocate the number of
nodes in the cluster? How should I split the work?

## What are RDDs?
An RDD is a collection of elements in distributed systems context. In
Spark all the data items are in the form of RDDs. You are working with RDDs one
way of another. When you read a large data file RDDs are created, Spark then
provides mechanics to operate on these RDDs.
According to the O'Reilly's Book ``Learning Spark`` it is:
```
An RDD in Spark is simply an immutable distributed collection of objects. Each RDD
is split into multiple partitions, which may be computed on different nodes of the
cluster. RDDs can contain any type of Python, Java, or Scala objects, including user-
defined classes.
```

## How do you create RDDs from DataSet?
RDDs are commonly created by loading an external dataset that is stored in some
persistent storage. The most common practice is to directly load from an
external remote storage such as AWS S3.

Everything in Spark works within a ``Spark Context`` which is created using the
Application's config.

```python
from pyspark import SparkConf, SparkContext
conf = SparkConf().setMaster("local").setAppName("My App")
sc = SparkContext(conf = conf)
```
After creating a Spark Context, you can use it call a slew of APIs that the
context object supports. You can read a dataset as follows
```python
import csv
import StringIO

def read_line(line):
	"""Parse a CSV line"""
	sio = StringIO.StringIO(line)
	csv_rd = csv.DictReader(sio, fieldnames=["col1", "col2"])
	return csv_rd.next()

input = sc.textFile(file_path).map(read_line)
```
However, if you have a local file, you can pass the path to the API for reading
external files/datasets. The fun bit is that this path ``file_path`` below can
be a remote path such as S3 path as well. It even supports wildcards in these
remote paths too!

``s3://bucket_name/dir_name/*.csv``

Passing this to the API will read all the CSV files present under
directory/folder ``dir_name`` in S3's bucket ``bucket_name`` which is unique.

## How can you run queries on them?
Spark makes it convenient to work with structured and semi-structured data using
Spark SQL. Saprk SQL provides a DataFrame abstraction on the data loaded. This
can be viewed as a "poor man's" Pandas DataFrame since the access APIs are not
as rich as Pandas' interface provides. But Spark's DataFrames can be thought of
as Relational DataBase rather than a data structure in memory.
However, SparkSQL supports a variety of file formats. It supports SQL queries on
the DataFrames which provides a huge advantage for plugging into existing
systems that already work with DataBases. Such systems can automatically
leverage from the distributed nature of the RDDs.

## What are the disadvantages of using a Pandas DataFrames with PySpark?
Although you can use Pandas DataFrames in your Python code which can be
intermixed with PySpark, the underlying distributed-ness of the Spark Framework
cannot be leveraged since Pandas doesn't support the RDDs semantics therefore,
does not yield itself to Spark's Distributed Computing Paradigm. RDDs are
distributed behind the scenes from the moment they are created from a dataset,
therefore, allow for Spark's efficiency in dealing with them. External construct
like Pandas has to be explicitly operated upon to scale.


