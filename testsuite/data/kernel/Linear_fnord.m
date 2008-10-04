accuracy = 1e-08;
init_random = 42;
name = 'Linear';
data_type = 'double';
km_train = [0.949935657, 0.625909344, 0.959511134, 0.771951061, 0.498971033, 0.746396126, 0.932228645, 0.812289281, 0.711295591, 0.644272439, 0.472863074;0.625909344, 1.10392427, 1.0766527, 1.0133115, 0.72264065, 0.812366925, 0.694908436, 0.741461103, 0.910055658, 0.772604222, 0.654562988;0.959511134, 1.0766527, 1.3288576, 1.10011735, 0.774151295, 1.00738134, 1.02176748, 0.981888145, 1.04639608, 0.876983656, 0.762667425;0.771951061, 1.0133115, 1.10011735, 1.20228102, 0.667957707, 0.985344401, 0.80017782, 0.893792735, 0.841684123, 0.66260774, 0.70958253;0.498971033, 0.72264065, 0.774151295, 0.667957707, 0.668811149, 0.581887761, 0.516465381, 0.55387919, 0.696625836, 0.656175168, 0.490471223;0.746396126, 0.812366925, 1.00738134, 0.985344401, 0.581887761, 1.04141055, 0.708327354, 0.804893214, 0.764709708, 0.584618973, 0.605059452;0.932228645, 0.694908436, 1.02176748, 0.80017782, 0.516465381, 0.708327354, 1.02253487, 0.91616816, 0.748738164, 0.70794992, 0.569626346;0.812289281, 0.741461103, 0.981888145, 0.893792735, 0.55387919, 0.804893214, 0.91616816, 1.03660104, 0.730775247, 0.60512688, 0.605575068;0.711295591, 0.910055658, 1.04639608, 0.841684123, 0.696625836, 0.764709708, 0.748738164, 0.730775247, 0.915682008, 0.810337142, 0.670235662;0.644272439, 0.772604222, 0.876983656, 0.66260774, 0.656175168, 0.584618973, 0.70794992, 0.60512688, 0.810337142, 0.967059584, 0.662303043;0.472863074, 0.654562988, 0.762667425, 0.70958253, 0.490471223, 0.605059452, 0.569626346, 0.605575068, 0.670235662, 0.662303043, 0.762902259];
data_train = [0.0965872303, 0.803407827, 0.866126059, 0.583761421, 0.366000548, 0.652629636, 0.305664073, 0.238385692, 0.48814521, 0.216153828, 0.483703608;0.956606505, 0.839784036, 0.94163092, 0.806328975, 0.354750936, 0.812045239, 0.922915744, 0.947035788, 0.683197851, 0.211366644, 0.0397291534;0.147200228, 0.965546918, 0.462096824, 0.854986015, 0.465044154, 0.660817583, 0.0241407771, 0.249864943, 0.43626928, 0.630631047, 0.171521188;0.764551464, 0.364404706, 0.817435269, 0.0979690904, 0.679756588, 0.293328333, 0.795383174, 0.402170427, 0.665349632, 0.934305269, 0.020626632;0.0111167823, 0.770279252, 0.646393052, 0.292979721, 0.494396703, 0.0419356656, 0.192445605, 0.424496375, 0.834044416, 0.479667183, 0.586362505;0.873753792, 0.308818914, 0.700699321, 0.861667259, 0.202615701, 0.356478165, 0.979810174, 0.983218565, 0.22322993, 0.0664458389, 0.270921611;0.397731957, 0.441005601, 0.618806307, 0.89950276, 0.330348012, 0.849984535, 0.0934789209, 0.041874699, 0.504045183, 0.0596622233, 0.392489801;0.896778117, 0.241483036, 0.705975775, 0.302365392, 0.0449200552, 0.556690581, 0.834853491, 0.409468448, 0.617293826, 0.900023598, 0.789694513;0.193876399, 0.733383959, 0.501893025, 0.745966808, 0.494515551, 0.127958164, 0.569909447, 0.492133992, 0.517921734, 0.878902454, 0.894837047;0.453537107, 0.455732041, 0.515186959, 0.519589278, 0.950387144, 0.649400467, 0.277456995, 0.531536797, 0.627223583, 0.639771721, 0.585254226;0.177736155, 0.0335931891, 0.337843052, 0.427522155, 0.0618042396, 0.814967338, 0.344077438, 0.913632746, 0.154410108, 0.0569349569, 0.482316104];
feature_class = 'simple';
data_test = [0.189129337, 0.937399276, 0.0103033985, 0.412619675, 0.926747771, 0.384418207, 0.0931092806, 0.302894943, 0.498010394, 0.185151687, 0.466561705, 0.836343196, 0.87261797, 0.0492607347, 0.160903358, 0.275290136, 0.594865851;0.360994759, 0.844040913, 0.716671868, 0.100456023, 0.624557503, 0.113012625, 0.25503567, 0.177870634, 0.173061949, 0.616395525, 0.563604923, 0.176028153, 0.990172805, 0.421836732, 0.0970693282, 0.120569017, 0.583893714;0.979069403, 0.571951102, 0.949041477, 0.444132851, 0.435841104, 0.500007359, 0.697425458, 0.367615966, 0.863886033, 0.0292813848, 0.439764195, 0.51099185, 0.607032238, 0.276071912, 0.427479971, 0.48453775, 0.686909979;0.773544185, 0.737827327, 0.341583052, 0.999450523, 0.848665312, 0.260023373, 0.195105806, 0.314021251, 0.452344853, 0.850799378, 0.778362264, 0.851246124, 0.271957115, 0.593461222, 0.283249062, 0.109023161, 0.684197743;0.142997551, 0.667697762, 0.647964305, 0.810248444, 0.139554605, 0.783480149, 0.999276526, 0.851042449, 0.751812076, 0.662165355, 0.421781485, 0.730500168, 0.724178035, 0.631498283, 0.796278378, 0.448763888, 0.121252712;0.559067728, 0.265043529, 0.384433097, 0.0174149039, 0.382790588, 0.808801106, 0.542722367, 0.00980052693, 0.46984382, 0.291451648, 0.477550655, 0.404692721, 0.93197922, 0.041177299, 0.683588011, 0.851026596, 0.959238585;0.177636227, 0.531304802, 0.0435895773, 0.499183524, 0.12545809, 0.837896675, 0.113870003, 0.51328836, 0.521096069, 0.971650498, 0.654980694, 0.287840293, 0.24079931, 0.128248206, 0.756514724, 0.690756656, 0.609877127;0.0408404429, 0.762828866, 0.641887577, 0.963888868, 0.58720213, 0.303951907, 0.381520552, 0.175840705, 0.722914182, 0.70772166, 0.200650015, 0.158489323, 0.0770049332, 0.911975744, 0.233069834, 0.344163527, 0.280504508;0.413624603, 0.714048799, 0.870595634, 0.18259875, 0.686412344, 0.352562142, 0.980527168, 0.298919159, 0.00719295601, 0.515639974, 0.150731084, 0.0444944268, 0.698269232, 0.732259283, 0.460699013, 0.676885517, 0.538239448;0.629801108, 0.596703973, 0.665822764, 0.199630649, 0.26870662, 0.43996315, 0.446128009, 0.774411712, 0.367393659, 0.162505156, 0.25146597, 0.214733502, 0.731660019, 0.470339615, 0.708630698, 0.416219681, 0.889735603;0.983520308, 0.669842509, 0.863812557, 0.507933267, 0.93462084, 0.831586486, 0.991778572, 0.623093735, 0.77920674, 0.54286814, 0.259120104, 0.354755171, 0.894005415, 0.0559479466, 0.591479886, 0.372484033, 0.386692538];
km_test = [0.601765752, 0.863134269, 0.712231756, 0.610296938, 0.735297836, 0.579561503, 0.523522897, 0.37481104, 0.609966466, 0.75737791, 0.617540643, 0.474127319, 0.791393356, 0.586329118, 0.543297335, 0.541953027, 0.847441233;0.720016792, 1.10712228, 0.918320248, 0.681423192, 0.828284583, 0.743619635, 0.825501681, 0.640890867, 0.772505451, 0.719449055, 0.699354372, 0.704385933, 1.09312958, 0.647148146, 0.705244321, 0.679157453, 0.920047895;0.82734722, 1.30482713, 0.97957924, 0.905318158, 1.05853907, 0.907514772, 0.867657808, 0.703314866, 0.932699719, 0.997645722, 0.866485902, 0.839433481, 1.22254644, 0.764349181, 0.829122758, 0.780556495, 1.110085;0.824829967, 1.0896077, 0.954338753, 0.605085047, 0.864196299, 0.911256891, 0.871759543, 0.626671935, 0.821768787, 0.792929565, 0.731071398, 0.62310307, 1.18135951, 0.56198267, 0.834800234, 0.848263138, 1.07515565;0.615126295, 0.797622149, 0.669714319, 0.536990037, 0.593738764, 0.558323682, 0.594589725, 0.559742632, 0.54478339, 0.554275186, 0.528439042, 0.532115767, 0.770281355, 0.515369003, 0.595837101, 0.49339384, 0.756786226;0.788933639, 1.05311766, 0.839896548, 0.673969772, 0.873896165, 0.796338094, 0.686994337, 0.618536247, 0.830649986, 0.768172542, 0.680868928, 0.596669178, 1.01078474, 0.49327661, 0.699491978, 0.643191436, 0.941569053;0.646857878, 0.953265286, 0.801221951, 0.641028908, 0.870569758, 0.625445066, 0.675955104, 0.398958265, 0.625137905, 0.786344056, 0.622750338, 0.540894735, 0.930554305, 0.641643473, 0.571711036, 0.590528923, 0.866838704;0.810776365, 0.974189716, 0.953557506, 0.576478646, 0.88187291, 0.775410237, 0.877193275, 0.551757376, 0.723600102, 0.729400698, 0.610782844, 0.559824078, 1.13554605, 0.556200782, 0.707996088, 0.655392478, 0.905878783;0.645766673, 1.06976034, 0.856016059, 0.794409761, 0.788091766, 0.722762147, 0.764490626, 0.660294953, 0.762456809, 0.83041061, 0.675364723, 0.665177642, 0.938182007, 0.71926448, 0.709098885, 0.617258981, 0.843707277;0.654427492, 0.950799432, 0.868393349, 0.78774933, 0.768471495, 0.553324736, 0.743858718, 0.543277406, 0.66466348, 0.700364469, 0.534312592, 0.556635361, 0.715646102, 0.782806883, 0.582027055, 0.537335385, 0.753036132;0.49496588, 0.856947918, 0.752294562, 0.608593539, 0.668739736, 0.6732962, 0.773249531, 0.561207939, 0.631074707, 0.635477828, 0.413924257, 0.438305682, 0.776802785, 0.594375168, 0.647244915, 0.61568191, 0.651925261];
data_class = 'rand';
feature_type = 'Real';