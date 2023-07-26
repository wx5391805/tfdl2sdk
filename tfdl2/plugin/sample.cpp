#include "layer/IPlugin.h"

namespace tfdl {
    class TFDLSamplePlugin : public IPlugin {
    public:
        TFDLSamplePlugin () : IPlugin () {
        }

        void ForwardFloat(vector <float*> &inputs, vector <vector <int> > &inputDims,
                          vector <float*> &outputs, vector <vector <int> > &outputDims) {
            float *input = inputs[0], *output = outputs[0];
            int len = 1;
            for (int i : inputDims[0]) {
                len *= i;
            }
            float maxValue = input[0], sum = 0.0;
            for (int i = 0; i < len; i++) {
                maxValue = max(maxValue, input[i]);
            }
            for (int i = 0; i < len; i++) {
            	output[i] = input[i] / maxValue;
            }
        }
    };

    REGISTER_TFDL_PLUGIN(TFDLSamplePlugin);
}
