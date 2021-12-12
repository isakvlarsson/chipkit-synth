import sys

def get_frequencies():
    freqs = [] 
    for i in range(127):
        note = 440 * 2**((i-69)/12) # Note freq!
        freqs.append(note)
    return freqs

if __name__ == "__main__":
   freqs = get_frequencies()
   samplerate = int(sys.argv[1])
   list = "const int SAMPLES_PER_PERIOD[] = {"
   i = 0;
   for freq in freqs:
        if i != 0:
            list += ","
        list += str(round((samplerate/freq)))
        i += 1
   list += "};"
   print(list)

