### convert_new.sh使用说明

此脚本用于将caffe float模型转换并量化为TFDL int8模型，包含转换、模型优化和量化三个过程，其中转换工具通过prototxt和caffemodel产生TFDL float模型；优化工具将TFDL float模型进行优化，产生一个优化过后的TFDL float模型，此模型计算上等效于未经优化过的模型；最后量化工具根据使用者指定的算法，对优化过的模型进行量化，产生一个TFDL int8模型。

```bash
$./converter_new.sh --help
Usage:
-h, --help                       print help message
-p, --prototxt <args>            input prototxt file path
-m, --caffemodel <args>          input caffemodel file path
-o, --output <args>              output model name
-c, --calibration_mode <args>    choose which calibration model to use
-s, --calibration_set <args>     calibration image set, if binary_set and this are both not implemented, use calibration.bin
-b, --binary_set <args>          binary input data set, if calibration_set and this are both not implementer, use calibration.bin
-f, --same_config <args>         whether to keep same config in a eltwise layer, default to be true
-v, --coverage <args>            coverage value, if not implemented, use 0.9995
-y, --symmetrical <args>         whether to use symmetrical mode in MES calibration
-d, --debug <args>               whether to print debug information after calibration
-r, --rgb <args>                 whether to use RGB instead of BGR for input images
-u, --frugal <args>              whether to enable frugal inference mode
-w, --calibrate_weight <args>:   choose the level of weight calibration
-g, --preprocess_config <args>   preprocess_config, if preprocess_config is implemented, add ImagePreprocessLayer
-e, --expectant_cos <args>:      Optimize the network until the cosine similarity reaches the desired value. (in [-1, 1])
```

### 参数说明：

- **help**：查看调用说明

- **prototxt**：caffe模型对应prototxt的路径。

- **caffemodel**：caffe模型caffemodel的路径。

- **output**：输出模型的名字。

- **calibration_mode**：决定程序使用的calibration方式，目前可选0~4五种方式，分别为：

    - **0.Naive模式**：与tflite所使用的的calibration方式相同，通过inference多张图片记录每个data的最大值最小值计算该data的scale和zeroPoint；

    - **1.Mean模式**：相比方式0，每层用于计算scale和zeroPoint的min，max并非极值，而是各次inference得到极值的平均；

    - **2.Entropy模式**：通过批量inference得到整个图集上的data分布，并枚举一定数量的min，max用于表征此分布，通过遍历枚举实例，计算每组min，max所表征的分布于真实分布的交叉熵，并选择交叉熵最小的作为最终的min，max来计算scale和zeroPoint；

    - **3.Coverage模式**：通过统一的coverage阈值向内收缩，从而去除占整体activation极少部分的outlier，此程序中默认coverage为99.95%；

    - **4.MES模式**：基于MES评价标准，按照搜索方式寻找每一data上的最优min，max值，减少量化性能损失。

    - **5.AdaQuantSequential模式**：见下`AdaQuant`。

    - **6.AdaQuant模式**：`AdaQuant`是基于Habana（Habana Labs – An Intel company 1 , Caesarea, Israel）于2020年6月发表的文章*《Improving Post Training Neural Quantization: Layer-wise Calibration and Integer Programming》*提出的post training量化方法改进而来的量化方法。AdaQuant在一个较小（通常不超过50）的校准训练集上，利用`Nelder-Mead`顺序算法搜索网络中所有数据块及权重的量化参数缩放系数，使得量化网络的最终输出结果与全浮点模型的最终输出结果均方差（MeanSquareError）最小化，从而获得最优的量化参数。

        ​		`AdaQuant`的优点在于它不仅考虑了数据块量化参数的缩放，也考虑了权重量化参数的缩放。并且它的自动化程度很高；缺点在于通常它需要比较长的运算时间，也需要限制校准集的大小。

        ​		`AdaQuantSequential`与`AadQuant`的不同在于它不会再每个搜索步骤中都推演整个网络来比较量化网络和浮点网络的最终输出结果，而是仅仅推演当前搜索权重和数据块所在的层，来比较该层量化输出结果与浮点输出结果的均方差值，并基于此进行优化。相较于`AdaQuant`而言，它的运算速度更快，也能使用更大的校准集，但在性能上稍逊一筹。

    - **7.CosineSearch模式**：`CosineSearch`模式在每一个数据块上遍历0~4中的所有量化参数组合。在每一个搜索步骤中，基于当前组合量化浮点模型，并用得到的量化模型进行推演，最大化与浮点模型最终输出结果的余弦相似度，从而得到最优的量化参数。

- **calibration_set**：calibration所使用的图集，为可选项，如不填使用该目录下calibration.bin文件进行校准，calibration.bin内为100张imageNet图集，二进制文件的生成方式参见该目录下的calibrationBinary.py。

- **binary_set**：calibration所使用的二进制输入数据，是由用户自行生成的float32格式numpy文件。为可选项，如不填使用该目录下calibration.bin文件进行校准，calibration.bin内为100张imageNet图集。

- **same_config**：控制eltwise处理quantization config的方式，默认为关。TFDL支持多种速度、精度不同的eltwise方式，因此加入此变量来配合inference方式达到比较好的效果，若same_config = 1，converter会将一个eltwise的三端量化参数值强制设为一致（取最大范围），这样的好处是在计算时速度较快，但可能造成一定的精度损失；若same_config = 0则不会有此约束，此时默认的inference方式为查表（与tflite强对齐），速度略有下降，可以在inference时设置eltwise mode 为2，来获得最快的速度，但不与tflite对齐。设置eltwise mode的API调用可以参考TFDL使用手册。

- **coverage**：控制Coverage模式下的采样点位置，默认为0.9995，可以在0~1之间设置，不建议设置小于0.9的值，可能会造成模型性能丢失。

- **symmetrical**：控制MES模式下的搜索方式，按照对称或非对称方式进行搜索。

- **debug**：debug选项，开启后会打印量化后每层输出的min，max值。

- **rgb**：输入图片格式选项，模式使用BGR模式，若设置为1使用RGB。

- **frugal**：控制是否开启节省内存的网络推理模式，若开启后网络将不会以batch形式运行，而是在数据集上对每个输入数据分别推理。这种节省内存的模式用以解决网络结构庞大或数据集庞大时内存开销过大问题。节省内存模式仅适用于Naive，Mean和Coverage模式，若在其他模式下开启节省内存，按照Naive方式校准网络。节省内存模式与`自定义calibration方式`不兼容。

- **calibrate_weight**：校准weight的级别，若开启可以对网络中的weight采取与`calibration_mode`相同的校准方式。可以为：`0`，`1`或`2`；`0`表示不对任何weight使用校准；`1`表示对InnerProduct操作的weight进行校准；`2`表示对InnerProduct和Convolution的weight进行校准

- **preprocess_config**：预处理层参数文件，若设置，网络会根据配置文件生成图像预处理层

- **expectant_cos**：可以设置一个[-1, 1]之间的值，如果设置了这个值，那么会在量化结束后开启优化器，优化器一直运行到余弦相似度达到期望（或收敛）为止。

- **quant_calibrate**：是否结束完某个数据块的校准后使用得到的量化参数对这个层进行FakeQuantization。

- **weight_tuning**：是否对卷积的weight和bias进行微调（使用`Nelder-Mead`算法优化均方差函数）。

- **load_parameter**：从json文件导入calibration参数配置。

- **dump_parameter**：将当前calibration所使用的的参数配置到处为文件，便于下次直接从改文件恢复配置。

### 其他

- **froceMin和forceMax**：有时，无论何种的calibration算法都无法找到一个对于具体模型最佳的量化参数，因此TFDL提供了手动调整某层输出min，max的接口：只需直接在输出的json文件中某层的参数上添加：`"forceMin": [float number], "forceMax": [float numer]` 就可以相应地调整量化参数，从而实现更灵活的量化参数控制。

- **自定义calibration方式**：对于复杂的网络，有时单一的calibration方式无法获得最佳的性能，因此我们支持对于不同的层使用不同的calibration方式。用户可以通过修改config.json中CalibrationMethod的内容来修改某一类型操作或某一指定名称data的calibration方式，例如你可以：

    ```json
    "CalibrationMethod": {
    	"Convolution": "ENTROPY"
    }
    ```

    它表示整个网络中所有convolution操作将会按照`ENTROPY`方式进行calibration。或者你也可以：

    ```json
    "CalibrationMethod": {
    	"conv1": "ENTROPY",
        "pool1": "NAIVE"
    }
    ```

    它表示名为“conv1”的data将使用`ENTROPY`进行calibration，名为“pool1”的data将使用`NAIVE`方式进行calibration。

    对于每个指定calibration方式的对象，可以选择的calibration方式为：`NAIVE`，`MEAN`，`ENTROPY`和`COVERAGE`。