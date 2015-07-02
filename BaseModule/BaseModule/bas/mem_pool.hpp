#ifndef __MEM_POOL_HPP_2015_07_01__
#define __MEM_POOL_HPP_2015_07_01__
//	����ģ���ڴ��ʵ��
//	�ڴ���޷������ڴ���Ƭ
//	��Ҫ�ڷ����ʱ�������ƥ�䣬�Ծ�����ʡ�ռ�
//	�ڴ���޷�����һ���Է��䳬��10M������
#include <bio.hpp>
#include <vector>
#include <map>

namespace bas
{
	namespace detail
	{
		static const int block_size = 10 * 1024 * 1024;		//	10M

		//	mem_pool_manager_t �������С����
		struct block_t : bio_bas_t<block_t>
		{
			//	���Է������С����
			struct alloc_unit
			{
				int offset;			//	�������ʼλ�õ�ƫ��
				int size;			//	����Ԫ���С
				char bfree;			//	�Ƿ���е�Ԫ��
				alloc_unit* prev;	//	��һ�鵥Ԫ��λ��
				alloc_unit* next;	//	��һ�鵥Ԫ��λ��
			};

		public :
			block_t() : buf_(), total_size_(), free_count_(), head_() { i_init(); }
			~block_t() { i_uninit(); }

		public :
			//	TODO : �˴���������ƥ�䣬�������������һ���㹻��ĵ�Ԫ��
			void* alloc_buffer(int size)
			{
				alloc_unit* cur = head_;
				alloc_unit* prev = cur;
				//	��������������һ����Ԫ��
				while(!cur->bfree && cur->size < size)
				{
					prev = cur;
					cur = cur->next;
				}

				int offset = cur - head_;
				alloc_unit* next = cur + size + sizeof(alloc_unit);

				if(next == head_ + total_size_)
				{	//	ֱ�ӷ����������ڴ��
					i_write_head_info(cur, offset, size, 0, prev, 0);
					free_count_--;
				}
				else
				{	//	δ������
					if(total_size_ - offset <= sizeof(alloc_unit))
					{	//	ʣ��ռ䲻��
						i_write_head_info(cur, offset, size, 0, prev, 0);
						free_count_--;
					}
					else
					{
						int next_size = cur->size - size - sizeof(alloc_unit);
						alloc_unit* temp_next = cur->next;

						//	���±�����Ϣ
						i_write_head_info(cur, offset, size, 0, prev, next);

						//	������һ����Ϣ
						i_write_head_info(next, next - head_, next_size, 1, cur, temp_next);
					}
				}

				return cur + sizeof(alloc_unit);
			}

			void free_buffer(void* buf)
			{
				if(!buf ||
					buf < head_ + sizeof(alloc_unit) ||
					buf > head_ + total_size_) return;

				alloc_unit* cur = (alloc_unit*)((char*)buf - sizeof(alloc_unit));
				cur->bfree = 1;
				i_merge_unit(cur);
			}

			int get_total_size() { return total_size_; }
			int get_free_count() { return free_count_; }

		private :
			void i_init()
			{
				buf_ = new char[block_size];
				total_size_ = block_size;

				i_write_head_info(buf_, 0, total_size_, 1, 0, 0);
				head_ = (alloc_unit*)buf_;

				free_count_ = 1;
			}

			void i_uninit()
			{
				if(buf_) delete buf_;
			}

			void i_write_head_info(void* p, int offset, int size, char bfree, alloc_unit* prev, alloc_unit* next)
			{
				if(!p) return;
				alloc_unit* u = (alloc_unit*)p;
				u->offset = offset;
				u->size	  = size;
				u->bfree  = bfree;
				u->prev   = prev;
				u->next   = next;
			}

			void i_merge_unit(alloc_unit* cur)
			{	//	�ϲ���Ԫ�飬һ������·������ͷŵ�ʱ��
				//	ֻ����ǰ���еĵ�Ԫ�飬ʹ���еĵ�Ԫ�鲻���κδ���
				if(cur->prev == 0 && cur->next == 0) return;
				if(cur->bfree == 0) return;

				if(cur->prev->bfree && cur->next->bfree) {
					free_count_--;
				} else if(!cur->prev->bfree && !cur->next->bfree) {
					free_count_++;
				}

				while(cur->prev && cur->prev->bfree) cur = cur->prev;
				while(cur->next && cur->next->bfree)
				{
					cur->size = cur->size + cur->next->size + sizeof(alloc_unit);
					cur->next = cur->next->next;
				}

				if(cur->next)
				{
					cur->next->prev = cur;
				}
			}

		private :
			void*	buf_;			//	������
			int		total_size_;	//	�����ܴ�С
			int		free_count_;	//	δʹ�õ�Ԫ������
			alloc_unit* head_;		//	ͷ
		};

		//////////////////////////////////////////////////////////////////////////
		//	�ڴ�ع���
		struct mem_pool_manager_t : bio_bas_t<mem_pool_manager_t>
		{
		public :
			mem_pool_manager_t() {}
			~mem_pool_manager_t() {}

		public :
			void init(int count = 1)
			{
				block_t* block = new block_t;
				block_list_.push_back(block);
			}

			void* alloc(int size)
			{
				void* buf = 0;
				
				for(int i = 0; i < block_list_.size(); i++)
				{
					block_t* block = block_list_[i];
					if(block->get_free_count() == 0) continue;
					buf = block->alloc_buffer(size);
					buf_block_map_.insert(std::pair<void*, block_t*>(buf, block));
					break;
				}

				if(buf == 0)
				{
					block_t* block = new block_t;
					block_list_.push_back(block);
					buf = block->alloc_buffer(size);
				}

				return buf;
			}

			void free(void* buf)
			{
				if(!buf) return;
				std::map<void*, block_t*>::const_iterator iter;
				iter = buf_block_map_.find(buf);
				if(iter == buf_block_map_.end()) return;
				iter->second->free_buffer(buf);
			}

		private :

		private :
			std::vector<block_t*> block_list_;
			std::map<void*, block_t*> buf_block_map_;
		};
	}
}

#endif
