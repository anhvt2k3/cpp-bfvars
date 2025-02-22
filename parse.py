import os
import csv
import json
import typing as T

test_ids = (
    filename.split("-")[-1][:-4] 
    for filename 
    in os.listdir(".") 
    if "icisn-config" in filename
)

def get_conf(input_file):
    # Open the CSV file for reading
    with open(input_file, mode='r', newline='\n') as infile:
        reader = csv.reader(infile, delimiter="/")
        
        next(reader)
        conf_row = next(reader)

        conf = {
            "FilterId": str(conf_row[0]),
            "FilterSize": int(conf_row[1]),
            "Capacity": int(conf_row[2]),
            "NumHashFunctions": int(conf_row[3]),
            "FalsePositiveRate": float(conf_row[4]),
            "NumItemsAdded": int(conf_row[5]),
            "BucketSize": int(conf_row[6]),
            "HashAlgo": str(conf_row[7]),
            "HashScheme": str(conf_row[8]),
        }

        return conf
            
def get_result(input_file):
    # Open the CSV file for reading
    with open(input_file, mode='r', newline='\n') as infile:
        reader = csv.reader(infile, delimiter="/")

        # Skip the header line (first row)
        next(reader)
        records = 0
        # Iterate through each row in the CSV file
        for row in reader:
            if not row:
                continue
            
            if len(row) != 16:
                continue

            # Extract the test case (the first part before '/')
            test_case = row[1]
            
            # Extract the remaining parts (values after '/')
            other_parts = row[2:]

            # Map the other parts to the headers
            row_data = {
                "TestCase": test_case,
                "AddOrRemoveTime": float(other_parts[0]),
                "KeySet": other_parts[1].split(","),
                "NonkeySet": other_parts[2].split(","),
                "F1": int(other_parts[3]),
                "F2": int(other_parts[4]),
                "F3": int(other_parts[5]),
                "F4": int(other_parts[6]),
                "F5": int(other_parts[7]),
                "TestSize": int(other_parts[8]),
                "Accuracy": float(other_parts[9]),
                "TestTime": float(other_parts[10]),
                "NofCollision": float(other_parts[11]),
                "NofRemovable": float(other_parts[12]),
                "NofOperand": float(other_parts[13]),
            }
            records += 1

            # Append the row data to the parsed_data list
            yield row_data
            
    print(f"Exhausted {input_file}: [records={records}]")

final_results: dict[T.Any, list[dict[str, T.Any]]] = {}
for test_id in test_ids:
    # Input and output file paths
    conf_filename = f"icisn-config-{test_id}"
    result_filename = f"icisn-result-{test_id}"

    conf_csv_filename = f"{conf_filename}.csv"
    result_csv_filename = f"{result_filename}.csv"

    conf_json_filename = f"{conf_filename}.json"
    result_json_filename = f"{result_filename}.json"

    # Prepare a list to hold the parsed data
    parsed_data = []
    
    try:
        conf = get_conf(conf_csv_filename)
        frozen_conf = frozenset(conf.items())
        if frozen_conf not in final_results:
            final_results[frozen_conf] = []
    
        for row in get_result(result_csv_filename):
            final_results[frozen_conf].append(row)
    except StopIteration:
        pass
        
with open("parsed.json", "w") as json_file:
    json_data = []
    for frozen_conf, final_result in final_results.items():
        dict_conf = dict(frozen_conf)
        conf = {k: dict_conf[k] for k in sorted(dict_conf)}
        data = {"keys": [], "values": []}
        for row in final_result:
            k = list(row.keys())
            v = list(row.values())
            data["keys"] = k
            data["values"].append(v)

        json_data.append({"conf": conf, "data": data})

    json.dump(
        json_data, 
        json_file, 
        indent=None,
    )
