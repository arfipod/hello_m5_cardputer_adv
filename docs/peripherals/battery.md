# Battery

Battery voltage is measured through ADC on G10.

The current implementation reports raw ADC, estimated voltage and an approximate
Li-ion percentage. Voltage is the reliable first value; percentage is only a
rough estimate.
