import ROOT

def WeightCalculator(samples):
    # samples : list of (samples name, cross-section, branching ratio, number of event)

    integrated_luminosity = 41.53 # [ Unit : fb^1 ]

    for sampleName, xs, BR, nOfEvt in samples:
        ##xs = xs * 1000.0  # pb to fb
        weight = (integrated_luminosity * xs * BR) / nOfEvt

        print(f"{sampleName} --> nOfEvt: {nOfEvt}, xs: {xs}, BR: {BR}, Weight: {weight}")

sample_data = [
    ("QCD_HT50to100", 187700000.0 * 1000.0, 1.0, 39819368),
    ("QCD_HT100to200", 23500000.0 * 1000.0, 1.0, 80534025),
    ("QCD_HT200to300", 1552000.0 * 1000.0, 1.0, 60056309),
    ("QCD_HT300to500", 321100.0 * 1000.0, 1.0, 54770756),
    ("QCD_HT500to700", 30250.0 * 1000.0, 1.0, 60395873),
    ("QCD_HT700to1000", 6398.0 * 1000.0, 1.0, 47501834),
    ("QCD_HT1000to1500", 1122.0 * 1000.0, 1.0, 14164109),
    ("QCD_HT1500to2000", 109.4* 1000.0, 1.0, 12402197),
    ("QCD_HT2000toInf", 21.74* 1000.0, 1.0, 5614050),
]

WeightCalculator(sample_data)

