import re
from collections import defaultdict

def parse_log_file(log_content):
    # Split the content by the 'END' marker to get chunks of logs for each filter
    filter_logs = log_content.split('END')
    
    filters_data = []
    
    for log in filter_logs:
        if not log.strip():
            continue
        
        filter_data = defaultdict(dict)
        
        # Extract the filter type
        filter_type_match = re.search(r'Filter:\s+(\w+)', log)
        if filter_type_match:
            filter_data['filter_type'] = filter_type_match.group(1)
        
        # Extract the structure information
        structure_match = re.search(r'Keys:\s+(\d+)\s+NonKeys:\s+(\d+)', log)
        if structure_match:
            filter_data['structure'] = {
                'keys': int(structure_match.group(1)),
                'nonkeys': int(structure_match.group(2)),
            }
        
        # Extract elapsed time for adding keys
        adding_keys_match = re.search(r'Adding Elapsed time:\s+([\d.]+)', log)
        if adding_keys_match:
            filter_data['adding_of_800000_keys'] = {
                'elapsed_time': float(adding_keys_match.group(1))
            }
        
        # Extract check elapsed times for two tests
        check_times = re.findall(r'Check Elapsed time:\s+([\d.]+)', log)
        if check_times:
            filter_data['check_elapsed_times'] = {
                'test_1': float(check_times[0]),
                'test_2': float(check_times[1])
            }
        
        # Extract FN and FP counts, as well as accuracy and filter size
        false_negative_matches = re.findall(r'FN count for set\d:\s+(\d+)', log)
        false_positive_match = re.search(r'FP count for set5:\s+(\d+)', log)
        accuracy_match = re.search(r'Accuracy:\s+([\d.]+)', log)
        
        # Initialize FN and FP counts
        if false_negative_matches or false_positive_match or accuracy_match:
            false_negatives = sum([int(fn) for fn in false_negative_matches]) if false_negative_matches else 0
            false_positives = int(false_positive_match.group(1)) if false_positive_match else 0
            filter_data['test_details'] = {}
            
            filter_data['test_details']['fn_count'] = false_negatives
            filter_data['test_details']['fp_count'] = false_positives
            filter_data['test_details']['accuracy'] = float(accuracy_match.group(1))
        
        filter_size_match = re.search(r'Filter Size:\s+(\d+)', log)
        if filter_size_match:
            filter_data['test_details']['filter_size'] = int(filter_size_match.group(1))
        
        # Extract configuration details
        configuration_match = re.search(
            r'Capacity:\s+(\d+)\s+Number of Hash Functions:\s+(\d+)\s+False Positive Rate:\s+([\d.]+)\s+Number of Items Added:\s+(\d+)', log
        )
        if configuration_match:
            filter_data['configuration'] = {
                'capacity': int(configuration_match.group(1)),
                'num_hash_functions': int(configuration_match.group(2)),
                'false_positive_rate': float(configuration_match.group(3)),
                'num_items_added': int(configuration_match.group(4))
            }
        
        # Extract bucket configuration
        bucket_config_match = re.search(
            r'Bucket Size per:\s+(\d+)\s+\(Max value: \d+\)\s+Bucket Count:\s+(\d+)', log
        )
        if bucket_config_match:
            filter_data['configuration']['buckets_configuration'] = {
                'bucket_size_per': int(bucket_config_match.group(1)),
                'bucket_count': int(bucket_config_match.group(2)),
            }
        
        # Only add to final list if there's actual filter data
        if filter_data:
            filters_data.append(dict(filter_data))
    
    return filters_data


# Read and process the log file using utf-16 encoding
with open('log.txt', 'r', encoding='utf-16') as f:
    log_content = f.read()

filters_data = parse_log_file(log_content)

# Print the whole list of parsed filter data
print(filters_data)

import pandas as pd
table_data = []

for entry in filters_data:
    row = {
        'Filter Type': entry['filter_type'],
        'Keys': entry['structure']['keys'],
        'Non-Keys': entry['structure']['nonkeys'],
        'Adding Time (s)': entry['adding_of_800000_keys']['elapsed_time'],
        'Check Time Test 1 (s)': entry['check_elapsed_times']['test_1'],
        'Check Time Test 2 (s)': entry['check_elapsed_times']['test_2'],
        'FN Count': entry['test_details']['fn_count'],
        'FP Count': entry['test_details']['fp_count'],
        'Accuracy': entry['test_details']['accuracy'],
        'Filter Size': entry['test_details']['filter_size'],
        'Capacity': entry['configuration']['capacity'],
        'Num Hash Functions': entry['configuration']['num_hash_functions'],
        'False Positive Rate': entry['configuration']['false_positive_rate'],
        'Num Items Added': entry['configuration']['num_items_added'],
        'Bucket Size (per)': entry['configuration']['buckets_configuration']['bucket_size_per'],
        'Bucket Count': entry['configuration']['buckets_configuration']['bucket_count']
    }
    table_data.append(row)

# Create DataFrame from table_data
df = pd.DataFrame(table_data)

# Write to an Excel file
df.to_excel('filter_comparison.xlsx', index=False)

print('Excel file filter_comparison.xlsx has been created successfully!')
