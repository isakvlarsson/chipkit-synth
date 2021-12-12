import math
import sys
def gen_sawtooth(freq,samplerate, arr_len,ampl):
    line = "{"
    period = 1/freq;
    sample_period = period * samplerate
    sawtooth_end = arr_len - math.ceil(arr_len % sample_period)
    for i in (range(arr_len)):
        if i != 1:
            line += ","
        line += hex(round(ampl * (i % sample_period)/sample_period)) 
        if i > sawtooth_end:
            line += "0xFFFF"
    line += "} \n"
    return line


def gen_triangle(freq,samplerate, arr_len,ampl):
    line = "{"
    period = 1/freq;
    sample_period = period * samplerate
    triangle_end = arr_len - (arr_len % sample_period)
    for i in range(arr_len):
        if i != 1:
            line += ","
        val = i % sample_period
        if val > 0.5:
            percentage = (1- (i%(sample_period >> 1))/(sample_period >> 1))
            line += hex(round(ampl * percentage))
        else:
            percentage = (i%(sample_period >> 1))/(sample_period >> 1)
            line += hex(round(ampl * percentage)) 
        if i > triangle_end:
            line += "0x00"
    line += "} \n"
    return line

def gen_square(freq,samplerate, arr_len,ampl):
    line = "{"
    period = 1/freq;
    sample_period = period * samplerate
    square_end = arr_len - (arr_len % sample_period)
    for i in range(arr_len):
        if i != 1:
            line += ","
        val = i % sample_period
        if val > 0.5:
            line += hex(ampl)
        else:
            line += hex(0)
        if i > square_end:
            line += "0x00"
    line += "} \n"
    return line

def get_frequencies():
    freqs = [] 
    for i in range(127):
        note = 440 * 2**((i-69)/12) # Note freq!
        freqs.append(note)
    return freqs

def all_waves(type,freqs,samplerate,num_periods,amplitude):
    line = "const int saw_sample[] = {"
    # Array is two periods of the lowest freq: 440*2^(-69/12)
    arr_len = math.ceil(num_periods*(1/freqs[0])*samplerate)
    for freq in freqs:
        if type == "SAW":
            line += gen_sawtooth(freq,samplerate, arr_len,amplitude)
        elif type == "TRIANGLE":
            line += gen_triangle(freq,samplerate,arr_len,amplitude)
        else:
            line += gen_square(freq,samplerate,arr_len,amplitude)
    line += "};"
    return line



if __name__ == "__main__":
   freqs = get_frequencies()
   samplerate = int(sys.argv[1])
   type = sys.argv[2]
   line = all_waves(type,get_frequencies(),samplerate,1,255)
   print(line)
