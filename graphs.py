import csv

bestOne = None

with open('Results\\results_chry.txt', mode='r') as csv_file:
    csv_reader = csv.DictReader(csv_file)
    line_count = 0
    for row in csv_reader:
        if line_count == 0:
            print(f'Column names are {", ".join(row)}')
            #line_count += 1
        if (bestOne == None or bestOne["BitsPerChar"] > row["BitsPerChar"]):
            bestOne = row
        line_count += 1
    print(f'Processed {line_count} lines.')
    print(bestOne)