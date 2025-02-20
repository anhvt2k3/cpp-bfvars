import matplotlib.pyplot as plt
import sqlizer

"""
# Given data with multiple Bloom Filters
data = [
    {
        'filter_type': 'VariableIncrementBloomFilter',
        'structure': {
            'keys': 800000,
            'nonkeys': 200000
        },
        'adding_of_800000_keys': {
            'elapsed_time': 0.715829
        },
        'check_elapsed_times': {
            'test_1': 1.05917,
            'test_2': 0.94012
        },
        'test_details': {
            'fn_count': 0,
            'fp_count': 87968,
            'accuracy': 0.912032,
            'filter_size': 7668046
        },
        'configuration': {
            'capacity': 800000,
            'num_hash_functions': 6,
            'false_positive_rate': 0.01,
            'num_items_added': 800000,
            'buckets_configuration': {
                'bucket_size_per': 4,
                'bucket_count': 7668046
            }
        }
    },
    {
        'filter_type': 'OneHashingBloomFilter',
        'structure': {
            'keys': 800000,
            'nonkeys': 200000
        },
        'adding_of_800000_keys': {
            'elapsed_time': 0.345366
        },
        'check_elapsed_times': {
            'test_1': 0.388336,
            'test_2': 0.380897
        },
        'test_details': {
            'fn_count': 0,
            'fp_count': 2034,
            'accuracy': 0.997966,
            'filter_size': 7668046
        },
        'configuration': {
            'capacity': 800000,
            'num_hash_functions': 6,
            'false_positive_rate': 0.01,
            'num_items_added': 800000,
            'buckets_configuration': {
                'bucket_size_per': 1,
                'bucket_count': 7668046
            }
        }
    }
]
"""

def plot_filters(filters_data):
    # Extract filter names
    filter_names = [f['filter_type'] for f in filters_data]

    # Extract the data
    adding_times = [f['adding_of_800000_keys']['elapsed_time'] for f in filters_data]
    check_times_1 = [f['check_elapsed_times']['test_1'] for f in filters_data]
    check_times_2 = [f['check_elapsed_times']['test_2'] for f in filters_data]
    fp_counts = [f['test_details']['fp_count'] for f in filters_data]
    accuracy = [f['test_details']['accuracy'] for f in filters_data]

    # Create subplots for each comparison
    fig, axs = plt.subplots(2, 2, figsize=(12, 10))

    # Automatically adjust the limits based on data ranges
    max_adding_time = max(adding_times)
    max_check_time = max(max(check_times_1), max(check_times_2))
    max_fp_count = max(fp_counts)
    print (max_fp_count) #! max_fp_count = 0??
    min_accuracy = min(accuracy)

    # Plot for adding elapsed time
    axs[0, 0].bar(filter_names, adding_times)
    axs[0, 0].set_title('Adding Time Comparison')
    axs[0, 0].set_ylabel('Time (seconds)')
    axs[0, 0].set_ylim(0, max_adding_time * 1.1)  # Add 10% buffer for visibility

    # Plot for check elapsed time
    axs[0, 1].bar(filter_names, check_times_1, label='Check Time 1', alpha=0.7)
    axs[0, 1].bar(filter_names, check_times_2, label='Check Time 2', alpha=0.7)
    axs[0, 1].set_title('Check Time Comparison')
    axs[0, 1].set_ylabel('Time (seconds)')
    axs[0, 1].set_ylim(0, max_check_time * 1.1)  # Add 10% buffer for visibility
    axs[0, 1].legend()

    # Plot for false positive count
    axs[1, 0].bar(filter_names, fp_counts)
    axs[1, 0].set_title('False Positive Count Comparison')
    axs[1, 0].set_ylabel('False Positives')
    axs[1, 0].set_ylim(0, max_fp_count * 1.1)  # Add 10% buffer for visibility

    # Plot for accuracy
    axs[1, 1].plot(filter_names, accuracy, marker='o')
    axs[1, 1].set_title('Accuracy Comparison')
    axs[1, 1].set_ylabel('Accuracy')
    axs[1, 1].set_ylim(min_accuracy * 0.9, 1.0)  # Show range from min accuracy to 1.0

    # Adjust layout and show plot
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    data = sqlizer.parse_log_file()
    plot_filters(data)