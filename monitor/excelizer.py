import sqlizer

def write_excel(filters_data):
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
    
if __name__ == '__main__':
    sqlizer_data = sqlizer.parse_log_file()
    write_excel(sqlizer_data)